#include "lvgl_sim.h"
#include <stdio.h>

int main(int argc, char* argv[])
{
    if (!monitor_init())
    {
        printf("模拟器初始化失败\n");
        return -1;
    }

    printf("初始化LVGL...\n");
    lv_init();

    // 初始化LVGL显示和输入设备驱动
    lvgl_port_init();


    custom_font_init();  // 初始化自定义字体

    // 创建UI控件
    create_ui_elements();

    
    printf("进入主循环...\n");

    while (1) {
        sdl_event_handler();
        lv_timer_handler();
        lv_task_handler();
        lv_tick_inc(5);
        soft_timer_handler();  // 调用UI元素中的软件定时器处理函数
        
        // 更新帧率显示
        update_fps_counter();
        
        SDL_Delay(5);
    }

    monitor_deinit();

    return 0;
}