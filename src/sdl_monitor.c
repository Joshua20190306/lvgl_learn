#include "lvgl_sim.h"

/* 静态变量用于SDL */
static SDL_Window* window = NULL;      // SDL 窗口
static SDL_Renderer* renderer = NULL;  // SDL 渲染器
static SDL_Texture* texture = NULL;    // SDL 纹理
static lv_color_t* fb = NULL;          // LVGL 帧缓冲区

static int mouse_x = 0;
static int mouse_y = 0;
static bool mouse_pressed = false;

static uint32_t last_key = 0;          // 存储最后按下的键
static bool key_pressed = false;       // 记录按键状态

/**
 * LVGL显示驱动的刷新回调函数。
 * 此函数使用渲染的内容更新屏幕。
 */
void flush_cb(lv_disp_drv_t* disp, const lv_area_t* area, lv_color_t* color_p)
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

/**
 * LVGL输入设备驱动的鼠标读取回调函数。
 * 此函数读取鼠标状态和位置。
 */
void mouse_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    data->point.x = mouse_x;
    data->point.y = mouse_y;

    if(mouse_pressed)
        data->state = LV_INDEV_STATE_PR;
    else
        data->state = LV_INDEV_STATE_REL;
}

/**
 * LVGL输入设备驱动的键盘读取回调函数。
 * 此函数读取键盘状态。
 */
void keyboard_read_cb(lv_indev_drv_t *indev_drv, lv_indev_data_t *data)
{
    if(key_pressed) {
        data->state = LV_INDEV_STATE_PR;
        data->key = last_key;
    } else {
        data->state = LV_INDEV_STATE_REL;
    }
}

/**
 * 处理SDL事件，例如鼠标移动和点击。
 */
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

            case SDL_KEYDOWN: {
                key_pressed = true;
                
                // 将SDL键码转换为LVGL键码
                switch(event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        last_key = LV_KEY_ESC;
                        break;
                    case SDLK_BACKSPACE:
                        last_key = LV_KEY_BACKSPACE;
                        break;
                    case SDLK_DELETE:
                        last_key = LV_KEY_DEL;
                        break;
                    case SDLK_UP:
                        last_key = LV_KEY_UP;
                        break;
                    case SDLK_DOWN:
                        last_key = LV_KEY_DOWN;
                        break;
                    case SDLK_RIGHT:
                        last_key = LV_KEY_RIGHT;
                        break;
                    case SDLK_LEFT:
                        last_key = LV_KEY_LEFT;
                        break;
                    case SDLK_HOME:
                        last_key = LV_KEY_HOME;
                        break;
                    case SDLK_END:
                        last_key = LV_KEY_END;
                        break;
                    default:
                        // 对于普通字符键，可以直接使用ASCII值
                        if((event.key.keysym.sym >= 32) && (event.key.keysym.sym <= 125)) {
                            last_key = event.key.keysym.sym;
                        } else {
                            // 未识别的键，返回ESC作为默认值
                            last_key = LV_KEY_ESC;
                        }
                        break;
                }
                break;
            }

            case SDL_KEYUP: {
                key_pressed = false;
                break;
            }

            case SDL_QUIT:
                printf("手动退出程序\n");
                monitor_deinit();
                exit(0);
                break;
        }
    }
}

/**
 * 初始化SDL显示窗口和渲染器。
 */
bool monitor_init(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL初始化失败: %s\n", SDL_GetError());
        return false;
    }
    
    // 启用垂直同步以减少画面撕裂
    // SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
    // 强制软件渲染 （不能动！）
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
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
    
    // 分配帧缓冲区
    fb = malloc(MONITOR_HOR_RES * MONITOR_VER_RES * sizeof(lv_color_t));
    if (!fb) {
        printf("帧缓冲区分配失败\n");
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return false;
    }

    return true;
}

/**
 * 返回指向帧缓冲区的指针
 */
lv_color_t* get_frame_buffer(void)
{
    return fb;
}

/**
 * 反初始化并清理SDL资源。
 */
void monitor_deinit(void)
{
    if(fb) {
        free(fb);
        fb = NULL;
    }
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}