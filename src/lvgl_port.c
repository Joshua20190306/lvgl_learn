#include "lvgl_sim.h"

/* 静态变量存储驱动句柄 */
static lv_disp_drv_t disp_drv;
static lv_indev_drv_t indev_drv;
static lv_indev_drv_t kb_drv;       // 键盘输入设备驱动

/**
 * 初始化LVGL显示和输入设备驱动
 */
void lvgl_port_init(void)
{
    printf("初始化LVGL显示和输入设备驱动...\n");

    /* 初始化显示缓冲区 */
    static lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, get_frame_buffer(), NULL, MONITOR_HOR_RES * MONITOR_VER_RES);

    /* 初始化显示驱动 */
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = MONITOR_HOR_RES;
    disp_drv.ver_res = MONITOR_VER_RES;
    disp_drv.flush_cb = flush_cb;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    /* 初始化鼠标/触摸输入设备驱动 */
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = mouse_read_cb;
    lv_indev_drv_register(&indev_drv);

    /* 初始化键盘输入设备驱动 */
    lv_indev_drv_init(&kb_drv);
    kb_drv.type = LV_INDEV_TYPE_KEYPAD;
    kb_drv.read_cb = keyboard_read_cb;
    lv_indev_drv_register(&kb_drv);
}

/**
 * 获取显示驱动句柄
 */
lv_disp_drv_t* get_display_driver(void)
{
    return &disp_drv;
}

/**
 * 获取输入设备驱动句柄
 */
lv_indev_drv_t* get_input_device_driver(void)
{
    return &indev_drv;
}