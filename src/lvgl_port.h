#ifndef LVGL_PORT_H
#define LVGL_PORT_H

/* 显示和输入设备驱动初始化函数 */
void lvgl_port_init(void);

/* 显示驱动句柄 */
extern lv_disp_drv_t* get_display_driver(void);

/* 输入设备驱动句柄 */
extern lv_indev_drv_t* get_input_device_driver(void);

#endif /* LVGL_PORT_H */