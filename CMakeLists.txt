cmake_minimum_required(VERSION 3.20)

project(
  mispdlog
  VERSION 0.0.0.1
  LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

message(STATUS "This is PROJECT_SOURCE_DIR " ${PROJECT_SOURCE_DIR})
message(STATUS "This is PROJECT_BINARY_DIR " ${PROJECT_BINARY_DIR})

option(ENABLE_BACKWARD "Enable build with backward-cpp" ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
  message("Setting default build type to Debug")
endif()

if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

if(WIN32)
  message(STATUS "This is Windows")
  # Specify MSVC UTF-8 encoding
  add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
  add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
elseif(UNIX)
  message(STATUS "This is UNIX")
else()
  message(STATUS "This is UNKNOW OS")
endif()

# 坏的写法，最好是用更现代化的target_include_directories
# include_directories(${PROJECT_SOURCE_DIR}/include)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(third-party)
