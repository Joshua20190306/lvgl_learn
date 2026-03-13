// #include "lvgl.h"
// #include "lvgl_port.h"
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
#include <sys/stat.h>
#include "lvgl_sim.h"
#include "freetype_example.h"

#define FONT_CHINESE_PATH "/home/hdl/work/res/font/wqyMicroHeiProportional.ttf"
#define FONT_ENGLISH_PATH "/home/hdl/work/res/font/Courier.ttf"


typedef struct
{
    int size;//字体大小
    lv_font_t* font;//字体
} custom_font_info_t;

custom_font_info_t chinese_font_info[10] = { 0 };
custom_font_info_t english_font_info[10] = { 0 };

lv_font_t font_custom;


/**
 * 检查文件是否存在
 */
static bool file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}


void custom_font_init(void)
{
    for (size_t i = 0; i < 10; i++)
    {
        chinese_font_info[i].size = 8 + i * 2;
        english_font_info[i].size = 8 + i * 2;
        // printf("加载字体:  index %d 字号：%d\n", i, chinese_font_info[i].size);
    }


    lv_ft_info_t info;
    const char* font_path[] = { FONT_CHINESE_PATH, FONT_ENGLISH_PATH }; // 字体文件路径
    int font_path_num = sizeof(font_path) / sizeof(font_path[0]);
    for (int i = 0; i < font_path_num; i++)
    {
        if (!file_exists(font_path[i]))
        {
            printf("字体文件不存在: %s\n", font_path[i]);
            continue;
        }

        for (int j = 0; j < 10; j++)
        {
            // printf("加载字体: index %d 字号：%d  %s\n", j, chinese_font_info[j].size, font_path[i]);
            memset(&info, 0, sizeof(lv_ft_info_t));
            info.name = font_path[i];
            if(i == 0)
                info.weight = chinese_font_info[j].size;
            if(i == 1)
                info.weight = english_font_info[j].size;
            info.style = FT_FONT_STYLE_NORMAL; // 使用正确的宏
            info.mem = NULL;
            info.mem_size = 0;
            #if 1
            if (lv_ft_font_init(&info))
            {
                if(i == 0)
                {
                    chinese_font_info[j].font = info.font;
                    // memcpy(&chinese_font_info[j].font, info.font, sizeof(lv_font_t));
                    printf("成功加载字体: 字号：%d  %s\n",chinese_font_info[j].size, font_path[i]);
                }
                if (i == 1)
                {
                    english_font_info[j].font = info.font;
                    // memcpy(&english_font_info[j].font, info.font, sizeof(lv_font_t));
                    printf("成功加载字体: 字号：%d  %s\n",english_font_info[j].size, font_path[i]);
                }

            }
            else
            {
                if (i == 0)
                    printf("无法加载字体: 字号：%d  %s\n", chinese_font_info[j].size, font_path[i]);
                if (i == 1)
                    printf("无法加载字体: 字号：%d  %s\n", english_font_info[j].size, font_path[i]);
            }
            #endif
        }
    }
}




lv_font_t* get_custom_font(int font_size, int language_index)
{
    custom_font_info_t *font_info;
    if (language_index == 0)
    {
        font_info = chinese_font_info;
    }
    else if (language_index == 1)
    {
        font_info = english_font_info;
    }

    for (int i = 0; i < 10; i++)
    {
        if (font_info[i].size == font_size)
        {
            return font_info[i].font;
        }
    }
    printf("无法获取字体: 字号：%d\n", font_size);
    return &lv_font_montserrat_20;  // 默认使用20号字体
}





