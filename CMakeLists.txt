cmake_minimum_required(VERSION 3.10)
project(lvgl_sim C)

set(CMAKE_C_STANDARD 99)

# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Find PkgConfig and FreeType
find_package(PkgConfig REQUIRED)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(FREETYPE REQUIRED freetype2)
    message(STATUS "Found Freetype: ${FREETYPE_VERSION}")
endif()

# 设置FreeType的包含目录为全局变量，让子项目可以使用
set(FREETYPE_INCLUDE_DIRS ${FREETYPE_INCLUDE_DIRS} CACHE PATH "FreeType include directories")

# LVGL configuration
set(LV_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h CACHE STRING "" FORCE)

# LVGL as a subdirectory
add_subdirectory(lvgl)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/lvgl
    /usr/include/freetype2  # 直接添加FreeType头文件目录
)

# 设置LVGL库的FreeType包含目录
target_include_directories(lvgl SYSTEM PUBLIC /usr/include/freetype2)

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
    src/lv_font_wqy_14.c
    src/freetype_example.c      # 添加FreeType示例源文件
    src/freetype_example.h      # 添加FreeType示例头文件
    src/RED.c
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link libraries
target_link_libraries(${PROJECT_NAME}
    lvgl::lvgl
    ${SDL2_LIBRARIES}
    ${FREETYPE_LIBRARIES}
)

# Compiler flags
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()