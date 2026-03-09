/**
 * @file main.c
 * LVGL 简单示例程序
 * 使用 SDL2 作为显示后端在桌面环境中模拟运行 LVGL 界面
 */

#include "lvgl/lvgl.h"      // LVGL 图形库头文件
#include <SDL2/SDL.h>         // SDL2 多媒体库头文件
#include <unistd.h>           // UNIX 标准库，包含 usleep 函数
#include <time.h>             // 时间库，用于获取当前时间
#include <stdio.h>            // 标准输入输出库
#include <locale.h>           // 本地化库，用于设置字符编码
#include <wchar.h>            // 宽字符库

#define SCREEN_WIDTH 800       // 屏幕宽度
#define SCREEN_HEIGHT 480      // 屏幕高度

static SDL_Window *window;             // SDL 窗口指针
static SDL_Renderer *renderer;         // SDL 渲染器指针
static SDL_Texture *texture;           // SDL 纹理指针
static lv_color_t *buf;                // LVGL 显示缓冲区指针
static lv_disp_draw_buf_t draw_buf;     // LVGL 显示缓冲区结构体
static lv_disp_drv_t disp_drv;          // LVGL 显示驱动结构体
static lv_indev_drv_t indev_drv;        // LVGL 输入设备驱动结构体
static bool quit_flag = false;          // 退出标志
static lv_point_t mouse_point;           // 鼠标位置
static bool mouse_pressed = false;       // 鼠标按键状态
static uint8_t click_count = 0; // 全局点击计数器
static lv_obj_t *debug_label = NULL; // 全局调试标签指针

/**
 * @brief 按钮被按下事件回调函数
 */
static void btn_pressed_cb(lv_event_t *event)
{
    LV_UNUSED(event);
    printf("按钮被按下\n");
    fflush(stdout);
}

/**
 * @brief 按钮被释放事件回调函数
 */
static void btn_released_cb(lv_event_t *event)
{
    LV_UNUSED(event);
    printf("按钮被释放\n");
    fflush(stdout);
}

/**
 * @brief 按钮获得焦点事件回调函数
 */
static void btn_focused_cb(lv_event_t *event)
{
    LV_UNUSED(event);
    printf("按钮获得焦点\n");
    fflush(stdout);
}

/**
 * @brief 按钮失去焦点事件回调函数
 */
static void btn_defocused_cb(lv_event_t *event)
{
    LV_UNUSED(event);
    printf("按钮失去焦点\n");
    fflush(stdout);
}

/**
 * @brief 按钮点击事件回调函数
 * @param event 事件对象
 * @note 点击按钮时打印当前时间
 */
static void btn_event_cb(lv_event_t *event)
{
    LV_UNUSED(event); // 告诉编译器该参数有意未使用
    
    // 增加点击计数
    click_count++;
    
    // 更新调试标签
    if(debug_label != NULL) {
        char txt_buf[32];
        snprintf(txt_buf, sizeof(txt_buf), "Clicks: %d", click_count);
        lv_label_set_text_static(debug_label, txt_buf);
    }
    
    // 获取当前时间
    time_t now = time(NULL);
    struct tm *local = localtime(&now);
    
    // 格式化时间字符串
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local);
    
    // 打印当前时间
    printf("按钮被点击！当前时间: %s\n", time_str);
    fflush(stdout);  // 立即刷新输出缓冲区
    
    // 添加额外的调试信息
    printf("按钮事件回调函数被调用，总点击次数: %d\n", click_count);
    fflush(stdout);
}

/**
 * @brief 显示刷新回调函数
 * @param disp 显示驱动指针
 * @param area 刷新区域
 * @param color_p 像素数据指针
 * @note 该函数将 LVGL 渲染的内容显示到 SDL 窗口
 */
static void flush_cb(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    // 计算刷新区域的宽度和高度
    int32_t w = area->x2 - area->x1 + 1;

    // 更新 SDL 纹理数据
    SDL_UpdateTexture(texture, NULL, (void*)color_p, w * sizeof(lv_color_t));
    // 清除渲染器
    SDL_RenderClear(renderer);
    // 复制纹理到渲染器
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    // 呈现到屏幕
    SDL_RenderPresent(renderer);

    // 通知 LVGL 刷新完成
    lv_disp_flush_ready(disp);
}

/**
 * @brief 输入设备读取回调函数
 * @param indev 输入设备驱动指针
 * @param data 输入数据指针
 * @note 读取鼠标位置和按键状态
 */
static void mouse_read_cb(lv_indev_drv_t *indev, lv_indev_data_t *data)
{
    LV_UNUSED(indev); // 告诉编译器该参数有意未使用
    
    data->point.x = mouse_point.x;  // 设置鼠标X位置
    data->point.y = mouse_point.y;  // 设置鼠标Y位置
    data->state = mouse_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
    
    // 注意：对于鼠标/触摸屏等持续输入设备，
    // 不应设置 data->continue_reading = true;
    // 让LVGL在需要时调用读取回调
}

void soft_timer_handler(void)
{
    static uint32_t count = 0;
    count++;
    if (count >= 200)
    {
        count = 0;
        // printf("定时器回调函数被调用\n");
    }
}


int main(void)
{
    /* 初始化 LVGL 库 */
    lv_init();

    /* 设置本地化环境，使用 UTF-8 编码 */
    setlocale(LC_ALL, "C.UTF-8");

    /* 初始化 SDL 视频和输入子系统 */
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);

    
    /* 创建 SDL 窗口，先使用英文标题 */
    window = SDL_CreateWindow("LVGL Simulator",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    
    /* 尝试设置中文标题 */
    SDL_SetWindowTitle(window, "LVGL 模拟器");
    
    /* 创建 SDL 渲染器 */
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    
    /* 创建 SDL 纹理，用于显示 LVGL 渲染的内容 */
    texture = SDL_CreateTexture(renderer,
                               SDL_PIXELFORMAT_RGB565,
                               SDL_TEXTUREACCESS_STREAMING,
                               SCREEN_WIDTH, SCREEN_HEIGHT);

    /* 初始化显示缓冲区 */
    buf = (lv_color_t*)malloc(sizeof(lv_color_t) * SCREEN_WIDTH * SCREEN_HEIGHT);
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, SCREEN_WIDTH * SCREEN_HEIGHT);

    /* 初始化显示驱动 */
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = SCREEN_WIDTH;           // 设置水平分辨率
    disp_drv.ver_res = SCREEN_HEIGHT;          // 设置垂直分辨率
    disp_drv.flush_cb = flush_cb;              // 设置刷新回调函数
    disp_drv.draw_buf = &draw_buf;             // 设置显示缓冲区
    lv_disp_drv_register(&disp_drv);           // 注册显示驱动

    /* 创建并注册输入设备（鼠标） */
    lv_indev_drv_init(&indev_drv);             // 初始化输入设备驱动
    indev_drv.type = LV_INDEV_TYPE_POINTER;    // 设置为指针设备（鼠标）
    indev_drv.read_cb = mouse_read_cb;         // 设置读取回调函数
    lv_indev_t * mouse_indev = lv_indev_drv_register(&indev_drv); // 注册输入设备驱动
    
    // 检查输入设备是否注册成功
    if(mouse_indev != NULL) {
        printf("输入设备注册成功\n");
        fflush(stdout);
        
        // 创建一个简单的光标
        lv_obj_t * cursor = lv_obj_create(lv_scr_act());
        lv_obj_set_size(cursor, 20, 20);
        lv_obj_set_style_bg_color(cursor, lv_palette_main(LV_PALETTE_RED), 0);
        lv_obj_clear_flag(cursor, LV_OBJ_FLAG_CLICKABLE); // 确保光标不会干扰点击事件
        lv_obj_clear_flag(cursor, LV_OBJ_FLAG_PRESS_LOCK); // 防止按下时光标锁定
        lv_indev_set_cursor(mouse_indev, cursor);
    } else {
        printf("输入设备注册失败\n");
        fflush(stdout);
    }

    /* 创建一个简单的标签 */
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Hello LVGL!");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);  // 居中显示

    /* 添加一个标签来显示按钮状态 */
    debug_label = lv_label_create(lv_scr_act());
    lv_label_set_text_static(debug_label, "Clicks: 0");
    lv_obj_align(debug_label, LV_ALIGN_TOP_LEFT, 10, 10);

    /* 创建一个按钮 */
    lv_obj_t *btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 120, 50);              // 设置按钮大小
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 50);  // 设置按钮位置（屏幕中心下方 50 像素）
    
    // 添加一些调试信息到按钮
    lv_obj_set_style_bg_color(btn, lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_obj_set_style_radius(btn, 5, 0);
    
    // 添加多个事件类型的监听器，以便更全面地调试
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);  // 添加点击事件回调
    lv_obj_add_event_cb(btn, btn_focused_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(btn, btn_defocused_cb, LV_EVENT_DEFOCUSED, NULL);
    lv_obj_add_event_cb(btn, btn_pressed_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(btn, btn_released_cb, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(btn, btn_focused_cb, LV_EVENT_FOCUSED, NULL);
    lv_obj_add_event_cb(btn, btn_defocused_cb, LV_EVENT_DEFOCUSED, NULL);
    lv_obj_add_event_cb(btn, btn_pressed_cb, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(btn, btn_released_cb, LV_EVENT_RELEASED, NULL);
    
    printf("按钮已创建，位置: 屏幕中央下方\n");
    fflush(stdout);

    /* 在按钮上创建标签 */
    lv_obj_t *btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Click me!");
    lv_obj_center(btn_label);                   // 标签居中

    /* 主循环 */
    while(!quit_flag) {
        /* 处理 SDL 事件 */
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                quit_flag = true;  // 收到退出事件，设置退出标志
            }
            /* 处理鼠标事件 */
            else if(event.type == SDL_MOUSEBUTTONDOWN) {
                if(event.button.button == SDL_BUTTON_LEFT) {
                    /* 更新鼠标位置和按键状态 */
                    mouse_point.x = event.button.x;
                    mouse_point.y = event.button.y;
                    mouse_pressed = true;
                    
                    /* 打印鼠标按下的坐标 */
                    printf("鼠标按下，坐标: (%d, %d)\n", (int)event.button.x, (int)event.button.y);
                    fflush(stdout);
                }
            }
            else if(event.type == SDL_MOUSEBUTTONUP) {
                if(event.button.button == SDL_BUTTON_LEFT) {
                    /* 更新鼠标位置和按键状态 */
                    mouse_point.x = event.button.x;
                    mouse_point.y = event.button.y;
                    mouse_pressed = false;
                    
                    /* 打印鼠标释放的坐标 */
                    printf("鼠标释放，坐标: (%d, %d)\n", (int)event.button.x, (int)event.button.y);
                    fflush(stdout);
                }
            }
            else if(event.type == SDL_MOUSEMOTION) {
                /* 更新鼠标位置 */
                mouse_point.x = event.motion.x;
                mouse_point.y = event.motion.y;
            }
        }
        
        /* 处理 LVGL 定时任务 */
        lv_timer_handler();
        usleep(1000);  // 休眠 1 毫秒
    }

    return 0;
}
