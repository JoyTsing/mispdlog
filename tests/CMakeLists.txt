file(GLOB_RECURSE all_tests CONFIGURE_DEPENDS *.cpp)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/test)

foreach(v ${all_tests})
  string(REGEX MATCH "tests/.*" relative_path ${v})
  string(REGEX REPLACE "tests/" "" target_name ${relative_path})
  string(REGEX REPLACE ".cpp" "" target_name ${target_name})

  add_executable(${target_name} ${v} ${all_srcs})
  target_link_libraries(${target_name} PUBLIC doctest nanobench mispdlog)
  if(ENABLE_BACKWARD AND NOT WIN32)
    target_link_libraries(
      ${target_name} PUBLIC Backward::Backward Backward::Object
                            Backward::Interface) # trace
  endif()
endforeach()
