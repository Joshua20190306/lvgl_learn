cmake_minimum_required(VERSION 3.10)
project(lvgl_sim C)

set(CMAKE_C_STANDARD 99)

# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# LVGL configuration
set(LV_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h CACHE STRING "" FORCE)

# LVGL as a subdirectory
add_subdirectory(lvgl)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl
)

# Source files
set(SOURCES
    src/main.c
    src/sdl_monitor.c
    src/sdl_monitor.h
    src/ui_elements.c
    src/ui_elements.h
    src/lvgl_port.c
    src/lvgl_port.h
    src/lvgl_sim.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    lvgl::lvgl
    ${SDL2_LIBRARIES}
)

# Compiler flags
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()