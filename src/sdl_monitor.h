#ifndef SDL_MONITOR_H
#define SDL_MONITOR_H

/* 显示配置 */
#define MONITOR_HOR_RES 480
#define MONITOR_VER_RES 480

/* 函数声明 */
bool monitor_init(void);           // 初始化显示器
void monitor_deinit(void);         // 反初始化显示器
void sdl_event_handler(void);      // SDL事件处理器
void flush_cb(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p);  // 刷新回调函数
void mouse_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data);                  // 鼠标读取回调函数
void keyboard_read_cb(lv_indev_drv_t *indev_drv, lv_indev_data_t *data);         // 键盘读取回调函数
lv_color_t* get_frame_buffer(void);  // 获取帧缓冲区

#endif /* SDL_MONITOR_H */