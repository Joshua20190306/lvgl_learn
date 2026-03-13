#ifndef LVGL_PORT_H
#define LVGL_PORT_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the LVGL display and input devices
 */
void lvgl_port_init(void);

// 新增FreeType字体加载函数声明
#if LV_USE_FREETYPE
/**
 * 初始化FreeType库（如果在lv_conf.h中启用）
 * @param max_faces 要缓存的最大字体数量
 * @param max_sizes 要缓存的最大字体大小数量
 * @param max_bytes 要缓存的最大字节数
 * @return true表示成功，false表示失败
 */
bool lv_freetype_port_init(uint32_t max_faces, uint32_t max_sizes, uint32_t max_bytes);

/**
 * 销毁FreeType库实例（如果在lv_conf.h中启用）
 */
void lv_freetype_port_destroy(void);
#endif

/* 显示驱动句柄 */
extern lv_disp_drv_t* get_display_driver(void);

/* 输入设备驱动句柄 */
extern lv_indev_drv_t* get_input_device_driver(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LVGL_PORT_H*/