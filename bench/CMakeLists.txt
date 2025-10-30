find_package(Threads REQUIRED)

file(GLOB_RECURSE all_benchs CONFIGURE_DEPENDS *.cpp)

# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/test)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/bench/logs)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/bench/results)

foreach(v ${all_benchs})
  string(REGEX MATCH "bench/.*" relative_path ${v})
  string(REGEX REPLACE "bench/" "" target_name ${relative_path})
  string(REGEX REPLACE ".cpp" "" target_name ${target_name})

  add_executable(${target_name} ${v} ${all_srcs})
  target_link_libraries(${target_name} PUBLIC nanobench mispdlog
                                              Threads::Threads)
endforeach()
