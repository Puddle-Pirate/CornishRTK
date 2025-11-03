# port/linux/CMakeLists.txt
find_package(Boost REQUIRED COMPONENTS context)

add_library(cornishrtk_port_impl STATIC
  port_linux_boost_continuation.cpp
)
target_compile_definitions(cornishrtk_port_impl PUBLIC RTK_SIMULATION=1)

target_link_libraries(cornishrtk_port_impl PUBLIC Boost::context)
target_include_directories(cornishrtk_port_impl PUBLIC
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/include
)
target_compile_features(cornishrtk_port_impl PUBLIC cxx_std_17)
