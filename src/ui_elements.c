#include "lvgl_sim.h"

/* 全局UI控件变量定义 */
lv_obj_t *g_title_label = NULL;

/* 文本输入框对象 */
static lv_obj_t *g_text_area = NULL;

/* 帧率显示标签 */
static lv_obj_t *g_fps_label = NULL;
static uint32_t frame_count = 0;
static uint32_t last_time = 0;

/* 全局键盘组 */
static lv_group_t *g_kb_group = NULL;

/* 用于标记是否已经设置了输入框焦点 */
static bool ta_focused = false;

/* 文本输入框事件回调函数 */
static void ta_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * ta = lv_event_get_target(e);
    
    if (code == LV_EVENT_FOCUSED)
    {
        // 检查是否已经处理过焦点事件，避免重复处理
        if(ta_focused) {
            return;
        }
        ta_focused = true;
        
        printf("文本输入框获得焦点\n");
        
        // 延迟初始化键盘组，确保在主线程中执行
        if(g_kb_group == NULL) {
            // 创建键盘组
            g_kb_group = lv_group_create();
            
            // 查找键盘类型的输入设备并将其与组关联
            lv_indev_t * indev = lv_indev_get_next(NULL);
            while(indev) {
                lv_indev_type_t type = lv_indev_get_type(indev);
                if(type == LV_INDEV_TYPE_KEYPAD) {
                    lv_indev_set_group(indev, g_kb_group);
                    break;
                }
                indev = lv_indev_get_next(indev);
            }
        }
        
        // 将输入框添加到组
        if(g_kb_group && ta) {
            lv_group_add_obj(g_kb_group, ta);
        }
    }
    else if (code == LV_EVENT_DEFOCUSED)
    {
        ta_focused = false;
        // 注释掉这部分，减少过多的输出
        // printf("文本输入框失去焦点\n");
    }
    else if(code == LV_EVENT_READY) {
        if(ta != NULL) {
            printf("文本输入框内容: %s\n", lv_textarea_get_text(ta));
        }
    }
}

/* 清空输入框内容的按钮回调函数 */
static void clear_ta_btn_cb(lv_event_t * e)
{
    if(e->code == LV_EVENT_CLICKED) {
        if(g_text_area != NULL) {
            lv_textarea_set_text(g_text_area, "");
            printf("已清空文本输入框内容\n");
        }
    }
}

/* 按钮事件回调函数 */
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

/* 滑块事件回调函数 */
static void slider_event_cb(lv_event_t *e)
{
    printf("滑动事件处理程序\n");
    lv_obj_t* slider = lv_event_get_target(e);
    lv_obj_t *label = lv_event_get_user_data(e);

    int32_t val = lv_slider_get_value(slider);
    lv_label_set_text_fmt(label, "value: %d", val);
}

extern const lv_font_t lv_font_wqy_14;



/* 创建UI元素 */
void create_ui_elements(void)
{
    printf("创建UI控件...\n");

    // 创建标题标签
    lv_obj_t *title_label = lv_label_create(lv_scr_act());
    lv_label_set_text(title_label, "LVGL Demo 15");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_20, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 20);
    g_title_label = title_label;

    // 创建帧率显示标签
    g_fps_label = lv_label_create(lv_scr_act());
    lv_label_set_text(g_fps_label, "FPS: --");
    lv_obj_set_style_text_font(g_fps_label, &lv_font_montserrat_20, 0);
    lv_obj_align(g_fps_label, LV_ALIGN_TOP_RIGHT, -10, 10);

    // 创建滑块
    lv_obj_t *slider = lv_slider_create(lv_scr_act());
    lv_obj_set_width(slider, 200);
    lv_obj_align(slider, LV_ALIGN_DEFAULT, 0, 100);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);

    // 创建滑块值标签
    lv_obj_t *slider_label = lv_label_create(lv_scr_act());
    lv_label_set_text(slider_label, "value: 50");
    lv_obj_align_to(slider_label, slider, LV_ALIGN_OUT_BOTTOM_MID, 0, 20);
    
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, slider_label);

    // 创建按钮
    lv_obj_t *btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(btn, 120, 50);
    lv_obj_align(btn, LV_ALIGN_DEFAULT, 0, 160);
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_PRESSED, NULL);
    
    lv_obj_t* btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "1234");
    lv_obj_center(btn_label);

    // 创建开关
    lv_obj_t *sw = lv_switch_create(lv_scr_act());
    lv_obj_align(sw, LV_ALIGN_DEFAULT, 0, 240);

    lv_obj_t *sw_label = lv_label_create(lv_scr_act());
    lv_label_set_text(sw_label, "switch");
    lv_obj_align_to(sw_label, sw, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);
    
    // 创建文本输入框
    g_text_area = lv_textarea_create(lv_scr_act());
    lv_obj_set_size(g_text_area, 200, 50);
    lv_obj_align(g_text_area, LV_ALIGN_DEFAULT, 0, 300);
    lv_textarea_set_placeholder_text(g_text_area, "input text here...");
    lv_obj_add_event_cb(g_text_area, ta_event_cb, LV_EVENT_ALL, NULL);
    
    // 创建清空输入框内容的按钮
    lv_obj_t *clear_btn = lv_btn_create(lv_scr_act());
    lv_obj_set_size(clear_btn, 100, 30);
    lv_obj_align(clear_btn, LV_ALIGN_DEFAULT, 0, 380);
    lv_obj_add_event_cb(clear_btn, clear_ta_btn_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t *clear_btn_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_btn_label, "clear");
    lv_obj_center(clear_btn_label);
    
    // 不自动设置焦点，让用户手动点击输入框

    // 创建系统中文标签
    lv_obj_t* chinese_label = lv_label_create(lv_scr_act());
    lv_label_set_text(chinese_label, "创建系统中文标签");
    lv_obj_set_style_text_font(chinese_label, &lv_font_simsun_16_cjk, 0);
    lv_obj_align(chinese_label, LV_ALIGN_DEFAULT, 0, 420);


 
    // 创建自定义中文标签  lv_font_wqy_14引入字符
    lv_obj_t* chinese_label1 = lv_label_create(lv_scr_act());
    lv_label_set_text(chinese_label1, "创建自定义中文标签");
    lv_obj_set_style_text_font(chinese_label1, &lv_font_wqy_14, 0);
    lv_obj_align(chinese_label1, LV_ALIGN_DEFAULT, 0, 440);


    // 创建ttf中文标签
    lv_obj_t* chinese_label2 = lv_label_create(lv_scr_act());
    lv_label_set_text(chinese_label2, "创建免费类型中文标签");
    lv_font_t* ttf_font = get_custom_font(14, 0);
    lv_obj_set_style_text_font(chinese_label2, ttf_font, 0);
    lv_obj_align(chinese_label2, LV_ALIGN_DEFAULT, 200, 300);

    // 创建ttf中文标签2
    lv_obj_t* chinese_label3 = lv_label_create(lv_scr_act());
    lv_label_set_text(chinese_label3, "创建免费类型中文标签2");
    ttf_font = get_custom_font(26, 0);
    lv_obj_set_style_text_font(chinese_label3, ttf_font, 0);
    lv_obj_align(chinese_label3, LV_ALIGN_DEFAULT, 200, 200);

    // 创建ttf 英文标签
    lv_obj_t* english_label = lv_label_create(lv_scr_act());
    lv_label_set_text(english_label, "creat ttf english label");
    lv_font_t* ttf_font1 = get_custom_font(14, 1);
    lv_obj_set_style_text_font(english_label, ttf_font1, 0);
    lv_obj_align(english_label, LV_ALIGN_DEFAULT, 200, 340);

    // 创建ttf 英文标签2
    lv_obj_t* english_label1 = lv_label_create(lv_scr_act());
    lv_label_set_text(english_label1, "creat ttf english label1");
    ttf_font1 = get_custom_font(26, 1);
    lv_obj_set_style_text_font(english_label1, ttf_font1, 0);
    lv_obj_align(english_label1, LV_ALIGN_DEFAULT, 200, 380);
    
    LV_IMG_DECLARE(RED);
    // 创建图片
    lv_obj_t* img = lv_img_create(lv_scr_act());
    lv_img_set_src(img, &RED);
    lv_obj_align(img, LV_ALIGN_DEFAULT, 200, 0);
}

/* 软件定时器处理函数 */
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

/* 帧率计算和更新函数 */
void update_fps_counter(void)
{
    frame_count++;
    
    // 每隔一定时间更新一次FPS显示
    uint32_t current_time = lv_tick_get();
    if(current_time - last_time >= 1000) { // 每秒更新一次
        float fps = (float)frame_count * 1000.0f / (current_time - last_time);
        
        if(g_fps_label != NULL) {
            lv_label_set_text_fmt(g_fps_label, "FPS: %d", (int)fps);
        }
        
        frame_count = 0;
        last_time = current_time; // 确保 last_time 被正确更新
    }
}