/**
 * Cornish Real-Time Kernel Application Programming Interface
 *
 *
 *
 *
*/
#ifndef _CORNISH_RTK_HPP_
#define _CORNISH_RTK_HPP_

#include <cstdint>
#include <port.h>

namespace rtk
{
   //-------------- Config ---------------
   static constexpr uint32_t MAX_PRIORITIES = 32; // 0 = highest, 32 = lowest
   static constexpr uint32_t TIME_SLICE     = 10; // In ticks


   struct Scheduler
   {
      static void init(uint32_t tick_hz);
      static void start();

      static void preempt_disable();
      static void preempt_enable();

      struct Lock
      {
         Lock()  { preempt_disable(); }
         ~Lock() { preempt_enable(); }
      };
   };


   class Thread
   {
   private:


   public:

   };

   class Mutex;

   class Semaphore;

   class SchedulerLock;

   class ConditionVar;



} // namespace rtk

#endif
