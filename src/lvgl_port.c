#include "lvgl_sim.h"
#include "lvgl_port.h"
#include "sdl_monitor.h"
#include "ui_elements.h"

/**
 * 获取显示驱动句柄
 */
lv_disp_drv_t* get_display_driver(void)
{
    static lv_disp_drv_t disp_drv;
    return &disp_drv;
}

/**
 * 获取输入设备驱动句柄
 */
lv_indev_drv_t* get_input_device_driver(void)
{
    static lv_indev_drv_t indev_drv;
    return &indev_drv;
}

/**
 * 初始化LVGL显示和输入设备驱动
 */
void lvgl_port_init(void)
{
    // 创建显示驱动的缓冲区
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf[MONITOR_HOR_RES * 10];  // 显存缓冲区
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, MONITOR_HOR_RES * 10);

    // 获取显示驱动并初始化
    lv_disp_drv_t *disp_drv = get_display_driver();
    lv_disp_drv_init(disp_drv);
    disp_drv->hor_res = MONITOR_HOR_RES;
    disp_drv->ver_res = MONITOR_VER_RES;
    disp_drv->flush_cb = flush_cb;  // 刷新回调函数
    disp_drv->draw_buf = &draw_buf;
    lv_disp_drv_register(disp_drv);

    // 创建输入设备驱动（鼠标/触摸板）
    static lv_indev_drv_t indev_drv_mouse;
    lv_indev_drv_init(&indev_drv_mouse);
    indev_drv_mouse.type = LV_INDEV_TYPE_POINTER;
    indev_drv_mouse.read_cb = mouse_read_cb;
    lv_indev_drv_register(&indev_drv_mouse);

    // 创建键盘输入设备驱动
    static lv_indev_drv_t indev_drv_kb;
    lv_indev_drv_init(&indev_drv_kb);
    indev_drv_kb.type = LV_INDEV_TYPE_KEYPAD;
    indev_drv_kb.read_cb = keyboard_read_cb;
    lv_indev_drv_register(&indev_drv_kb);
}

// 如果启用了FreeType，则提供便捷的初始化函数
#if LV_USE_FREETYPE
bool lv_freetype_port_init(uint32_t max_faces, uint32_t max_sizes, uint32_t max_bytes)
{
    return lv_freetype_init(max_faces, max_sizes, max_bytes);
}

void lv_freetype_port_destroy(void)
{
    lv_freetype_destroy();
}
#endif

