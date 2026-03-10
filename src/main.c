#include "lvgl/lvgl.h"
#include <SDL2/SDL.h>
#include <stdio.h>

#define MONITOR_HOR_RES 480
#define MONITOR_VER_RES 480

// monitor    数据方向：纹理->渲染器->窗口
static SDL_Window* window = NULL;      // SDL 窗口
static SDL_Renderer* renderer = NULL;  // SDL 渲染器
static SDL_Texture* texture = NULL;    // SDL 纹理

static lv_color_t* fb = NULL;          // LVGL frame buffer


static int mouse_x = 0;
static int mouse_y = 0;
static bool mouse_pressed = false;
lv_obj_t *g_title_label = NULL;


static void flush_cb(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p)
{
  
    // 1. 获取需要更新的区域大小
    int w = lv_area_get_width(area);
    int h = lv_area_get_height(area);

    // 2. 设置SDL更新区域
    SDL_Rect sdl_area = { 
        .x = area->x1,
        .y = area->y1,
        .w = w,
        .h = h,
    };

    // 计算刷新区域的宽度和高度
    printf("刷新位置：%d %d %d %d\n", sdl_area.x, sdl_area.y, sdl_area.w, sdl_area.h);
    // 更新 SDL 纹理数据
    SDL_UpdateTexture(texture, &sdl_area, (void*)color_p, w * sizeof(lv_color_t));
    // SDL_UpdateTexture(texture, NULL, (void*)color_p, w * sizeof(lv_color_t));
    // 清除渲染器
    SDL_RenderClear(renderer);
    // 复制纹理到渲染器
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    // 呈现到屏幕
    SDL_RenderPresent(renderer);

    // 通知 LVGL 刷新完成
    lv_disp_flush_ready(disp);
}

void mouse_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    data->point.x = mouse_x;
    data->point.y = mouse_y;

    if(mouse_pressed)
        data->state = LV_INDEV_STATE_PR;
    else
        data->state = LV_INDEV_STATE_REL;
}

static void btn_event_cb(lv_event_t *e)
{
    printf("点击事件处理程序\n");
    lv_obj_t* btn = lv_event_get_target(e);
    lv_obj_t *label = lv_obj_get_child(btn, 0);

    if (e->code == LV_EVENT_CLICKED)
    {
        static uint8_t cnt = 0;
        cnt++;
        // lv_label_set_text_fmt(label, "点击: %d", cnt);
        if(cnt%2)
            lv_label_set_text(label, "111");
        else
            lv_label_set_text(label, "222");
    }
}

static void slider_event_cb(lv_event_t *e)
{
    printf("滑动事件处理程序\n");
    lv_obj_t* slider = lv_event_get_target(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    int32_t val = lv_slider_get_value(slider);
    lv_label_set_text_fmt(label, "value: %d", val);
}

void monitor_deinit(void)
{
    free(fb);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void sdl_event_handler()
{
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_MOUSEMOTION:
                mouse_x = event.motion.x;
                mouse_y = event.motion.y;
                break;

            case SDL_MOUSEBUTTONDOWN:
                mouse_pressed = true;
                break;

            case SDL_MOUSEBUTTONUP:
                mouse_pressed = false;
                break;

            case SDL_QUIT:
                printf("手动退出程序\n");
                monitor_deinit();
                exit(0);
                break;
        }
    }
}


void soft_timer_handler(void)
{
    static uint8_t cnt = 0;
    static uint8_t last_cnt = 0;
    cnt++;
    if (cnt < 200)
        return;

    cnt = 0;
    last_cnt++;
    printf("定时器: %d\n", last_cnt);
    if (last_cnt % 2)
        lv_obj_add_flag(g_title_label, LV_OBJ_FLAG_HIDDEN);
    else
        lv_obj_clear_flag(g_title_label, LV_OBJ_FLAG_HIDDEN);
}

bool monitor_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL初始化失败: %s\n", SDL_GetError());
        return false;
    }
    // 强制软件渲染  
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");

    printf("创建窗口...\n");
    window = SDL_CreateWindow("LVGL Demo",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        MONITOR_HOR_RES,
        MONITOR_VER_RES,
        SDL_WINDOW_SHOWN);
    if (!window)
    {
        printf("窗口创建失败: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    printf("创建渲染器...\n");
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("渲染器创建失败: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    printf("创建纹理...\n");
    texture = SDL_CreateTexture(renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        MONITOR_HOR_RES,
        MONITOR_VER_RES);
    if (!texture)
    {
        printf("纹理创建失败: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }
    return true;
}

#define LVGL_TICK_PERIOD_MS 5

// 这个函数会在每个 tick 周期调用
void lv_tick_task(void) {
    lv_tick_inc(LVGL_TICK_PERIOD_MS);  // 增加系统时基

}


int main(int argc, char* argv[])
{
    if (!monitor_init())
    {
        printf("模拟器初始化失败\n");
        return -1;
    }

    fb = malloc(MONITOR_HOR_RES * MONITOR_VER_RES * sizeof(lv_color_t));
    if (!fb) {
        printf("帧缓冲分配失败\n");
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    printf("初始化LVGL...\n");
    lv_init();

    static lv_disp_draw_buf_t draw_buf;
    lv_disp_draw_buf_init(&draw_buf, fb, NULL, MONITOR_HOR_RES * MONITOR_VER_RES);

    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = MONITOR_HOR_RES;
    disp_drv.ver_res = MONITOR_VER_RES;
    disp_drv.flush_cb = flush_cb;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = mouse_read_cb;
    lv_indev_drv_register(&indev_drv);

    // SDL_TimerID timer_id = SDL_AddTimer(LVGL_TICK_PERIOD_MS, (SDL_TimerCallback)lv_tick_task, NULL);
    // if (!timer_id) {
    //     printf("SDL_AddTimer Error: %s\n", SDL_GetError());
    //     SDL_Quit();
    //     return -1;
    // }

    printf("创建UI控件...\n");

    // 创建标题标签
    lv_obj_t *title_label = lv_label_create(lv_scr_act());
    lv_label_set_text(title_label, "LVGL Demo 15");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 20);
    g_title_label = title_label;

    // 创建滑块
    lv_obj_t *slider = lv_slider_create(lv_scr_act());
    lv_obj_set_width(slider, 200);
    lv_obj_align(slider, LV_ALIGN_CENTER, 0, -50);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);

    // 创建滑块值标签
    lv_obj_t *slider_label = lv_label_create(lv_scr_act());
    lv_label_set_text(slider_label, "value: 50");
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, slider_label);

    // 创建按钮
    lv_obj_t *btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 120, 50);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 50);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_PRESSED, NULL);
    
    lv_obj_t* btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "1234");
    lv_obj_center(btn_label);


    // 创建开关
    lv_obj_t *sw = lv_switch_create(lv_scr_act());
    lv_obj_align(sw, LV_ALIGN_CENTER, 0, 120);

    lv_obj_t *sw_label = lv_label_create(lv_scr_act());
    lv_label_set_text(sw_label, "switch");
    lv_obj_align_to(sw_label, sw, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

    printf("进入主循环...\n");

    while (1) {
        sdl_event_handler();
        lv_timer_handler();
        lv_task_handler();
        lv_tick_inc(5);
        soft_timer_handler();
        SDL_Delay(5);
    }

cleanup:
    
    free(fb);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
