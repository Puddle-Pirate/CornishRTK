/**
 *
 */
#include <cassert>
#include <cornishrtk.hpp>

#include <atomic>
#include <cstdint>
#include <deque>
#include <array>

namespace rtk
{

   struct TaskControlBlock
   {
      enum class State : uint8_t {
         Ready,
         Running,
         Blocked,
         Suspended
      } state{State::Ready};
      uint8_t priority{0};
      uint32_t timeslice_left{0};
      uint32_t wake_tick{0};

      port_context_t* context{nullptr};
      void*  stack_base{nullptr};
      size_t stack_size{0};
      void (*entry_fn)(void);
      void*  arg{nullptr};
   };

   struct InternalSchedulerState
   {
      std::atomic<uint32_t> tick{0};
      std::atomic<bool> preempt_disabled{false};
      std::atomic<bool> need_reschedule{false};
      TaskControlBlock* current_task{nullptr};

      std::array<std::deque<TaskControlBlock*>, MAX_PRIORITIES> ready{}; // TODO: replace deque
      uint32_t ready_bitmap{0};
      std::deque<TaskControlBlock*> sleep_queue{}; // TODO: replace with wheel/heap later

      void reset()
      {
         tick.store(0);
         preempt_disabled.store(false);
         need_reschedule.store(false);
         current_task = nullptr;
      }
   };
   static /*constinit*/ InternalSchedulerState iss;

   static TaskControlBlock* pick_highest_ready()
   {
      if (!iss.ready_bitmap) return nullptr;
      uint8_t priority = std::countr_zero(iss.ready_bitmap);
      return iss.ready[priority].front();
   }

   static void remove_ready_head(uint8_t priority)
   {
      auto& queue = iss.ready[priority];
      queue.pop_front();
      if (queue.empty()) iss.ready_bitmap &= ~(1u << priority);
   }

   static void set_ready(TaskControlBlock* tcb)
   {
      tcb->state = TaskControlBlock::State::Ready;
      iss.ready[tcb->priority].push_back(tcb);
      iss.ready_bitmap |= (1u << tcb->priority);
   }

   static void context_switch_to(TaskControlBlock* next)
   {
      if (next == iss.current_task) return;
      TaskControlBlock* previous_task = iss.current_task;
      iss.current_task = next;
      if (previous_task) previous_task->state = TaskControlBlock::State::Ready;
      iss.current_task->state = TaskControlBlock::State::Running;
      port_switch(previous_task ? &previous_task->context : nullptr, iss.current_task->context);
   }

   static void schedule()
   {
      if (iss.preempt_disabled.load()) return;
      if (!iss.need_reschedule.exchange(false)) return;

      auto next = pick_highest_ready();
      remove_ready_head(next->priority);
      context_switch_to(next);
   }


   void Scheduler::init(uint32_t tick_hz)
   {
      iss.reset();
      port_init(tick_hz);
   }

   void Scheduler::start()
   {
      auto first = pick_highest_ready();
      assert(first != nullptr);
      remove_ready_head(first->priority);
      iss.current_task = first;
      iss.current_task->state = TaskControlBlock::State::Running;
      iss.current_task->timeslice_left = TIME_SLICE;

      port_start_first(iss.current_task->context);

      if constexpr (RTK_SIMULATION) {
         while (true) {
            schedule();
            struct timespec req{.tv_nsec = 1'000'000};
            nanosleep(&req, nullptr);
         }
      }
      __builtin_unreachable();
   }

   void Scheduler::preempt_disable() { port_preempt_disable(); }
   void Scheduler::preempt_enable()  { port_preempt_enable();  }

} // namespace rtk
