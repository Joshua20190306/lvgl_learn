#ifndef UI_ELEMENTS_H
#define UI_ELEMENTS_H

#include "lvgl_sim.h"

/* 全局UI控件变量声明 */
extern lv_obj_t *g_title_label;

/* 函数声明 */
void create_ui_elements(void);
void soft_timer_handler(void);
void update_fps_counter(void);  // 帧率更新函数

#endif