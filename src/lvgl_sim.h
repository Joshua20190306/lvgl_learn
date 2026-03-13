#ifndef LVGL_SIM_H
#define LVGL_SIM_H

#ifdef __cplusplus
extern "C" {
#endif

/* LVGL相关头文件 */
#include "lvgl/lvgl.h"

/* SDL相关头文件 */
#include <SDL2/SDL.h>

/* 标准库头文件 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

/* 项目模块头文件 */
#include "sdl_monitor.h"
#include "ui_elements.h"
#include "lvgl_port.h"
#include "freetype_example.h"

/* 显示配置 */
#define MONITOR_HOR_RES 480
#define MONITOR_VER_RES 480

/* LVGL Tick周期定义（毫秒）*/
#define LVGL_TICK_PERIOD_MS 5

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LVGL_SIM_H */