cmake_minimum_required(VERSION 3.10)
project(SDL3-App CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE APP_SOURCES "${SOURCE_DIR}/*.cpp")
add_executable(SDL3-App  ${APP_SOURCES})

add_subdirectory(vendor/sdl3)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/sdl_image)

target_include_directories(SDL3-App  PUBLIC ${SOURCE_DIR} vendor/sdl3 vendor/glm/glm vendor/sdl_image/include/sdl3_image)
target_link_libraries(SDL3-App PRIVATE SDL3::SDL3 glm::glm SDL3_image::SDL3_image)
