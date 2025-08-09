cmake_minimum_required(VERSION 3.28)
project(SDL3-App CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_compile_options(-Wall -Wextra -g)

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE APP_SOURCES 
    "${SOURCE_DIR}/*.cpp"
    "${SOURCE_DIR}/game/*.cpp")

add_executable(SDL3-App  ${APP_SOURCES})

add_subdirectory(vendor/sdl3)
add_subdirectory(vendor/glm)
add_subdirectory(vendor/sdl_image)
add_subdirectory(vendor/glew/build/cmake)

# --- ImGui Integration ---
set(IMGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/vendor/imgui")
target_sources(SDL3-App PRIVATE
    "${IMGUI_DIR}/imgui.cpp"
    "${IMGUI_DIR}/imgui_draw.cpp"
    "${IMGUI_DIR}/imgui_widgets.cpp"
    "${IMGUI_DIR}/imgui_tables.cpp"
    "${IMGUI_DIR}/imgui_demo.cpp"

    "${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp"
    "${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp"
    
)
target_include_directories(SDL3-App PRIVATE
    "${IMGUI_DIR}"
    "${IMGUI_DIR}/backends"
)

target_include_directories(SDL3-App PUBLIC ${SOURCE_DIR} vendor/sdl3 vendor/glm/glm vendor/sdl_image/include/sdl3_image vendor/glew/include)
target_link_libraries(SDL3-App PRIVATE SDL3::SDL3 glm::glm SDL3_image::SDL3_image glew)

