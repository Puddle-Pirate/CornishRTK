add_library(cornishrtk_kernel STATIC
  hooks_stub.cpp
  scheduler.cpp
)

target_link_libraries(cornishrtk_kernel PUBLIC cornishrtk_port_impl)
target_include_directories(cornishrtk_kernel PUBLIC ${CMAKE_SOURCE_DIR}/include)
