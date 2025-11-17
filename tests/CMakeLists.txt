# Example demo that uses the kernel
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/two_thread_equal_prio.cpp")
  add_executable(two_thread_equal_prio two_thread_equal_prio.cpp)
  target_link_libraries(two_thread_equal_prio PRIVATE cornishrtk_kernel)
  target_include_directories(two_thread_equal_prio PRIVATE ${CMAKE_SOURCE_DIR}/include)
  target_compile_features(two_thread_equal_prio PRIVATE cxx_std_23)
endif()
