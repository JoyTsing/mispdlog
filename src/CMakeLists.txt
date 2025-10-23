find_package(fmt REQUIRED)

file(GLOB_RECURSE all_hdrs CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE all_srcs CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(mispdlog STATIC ${all_srcs})

target_include_directories(
  mispdlog PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                  $<INSTALL_INTERFACE:include>)

target_link_libraries(mispdlog PUBLIC fmt::fmt)
