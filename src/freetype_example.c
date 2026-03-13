#include "lvgl.h"
#include "lvgl_port.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <libgen.h>
#include "lvgl_sim.h"
#include "freetype_example.h"

/* 字体文件路径（相对于项目根目录） */
#define FONT_CHINESE_PATH   "res/font/wqyMicroHeiProportional.ttf"
#define FONT_ENGLISH_PATH   "res/font/Courier.ttf"

/* 字体配置 */
#define FONT_SIZE_MIN       8       /* 最小字号 */
#define FONT_SIZE_MAX       26      /* 最大字号 */
#define FONT_SIZE_STEP      2       /* 字号步长 */
#define FONT_CACHE_SIZE     10      /* 字体缓存数量 */

/**
 * @brief 自定义字体信息结构体
 */
typedef struct
{
    int size;           /**< 字体大小 */
    lv_font_t* font;    /**< 字体指针 */
} custom_font_info_t;

/* 全局字体缓存 */
static custom_font_info_t chinese_font_cache[FONT_CACHE_SIZE] = {0};
static custom_font_info_t english_font_cache[FONT_CACHE_SIZE] = {0};

/**
 * @brief 检查文件是否存在
 * @param filename 文件路径
 * @return true 文件存在，false 文件不存在
 */
static bool file_exists(const char *filename)
{
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

/**
 * @brief 切换到项目根目录
 * 
 * 通过检测可执行文件路径，如果位于 build 目录中则切换到父目录
 * 确保资源文件路径解析正确
 */
static void change_to_project_root(void)
{
    char exe_path[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    
    if (len != -1) {
        exe_path[len] = '\0';
        
        /* 获取可执行文件所在目录 */
        char* dir_name = dirname(exe_path);
        char* base_name = basename(dir_name);
        
        /* 如果在 build 目录中，切换到父目录 */
        if (strcmp(base_name, "build") == 0) {
            if (chdir("..") == 0) {
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) != NULL) {
                    printf("[INFO] 已切换到项目根目录：%s\n", cwd);
                }
            } else {
                fprintf(stderr, "[ERROR] 切换目录失败\n");
            }
        }
    }
}

/**
 * @brief 初始化单个字体
 * @param font_path 字体文件路径
 * @param font_size 字体大小
 * @return lv_font_t* 字体指针，失败返回 NULL
 */
static lv_font_t* load_font(const char* font_path, int font_size)
{
    if (!font_path || !file_exists(font_path)) {
        fprintf(stderr, "[ERROR] 字体文件不存在：%s\n", font_path);
        return NULL;
    }

    lv_ft_info_t info = {0};
    info.name = font_path;
    info.weight = font_size;
    info.style = FT_FONT_STYLE_NORMAL;
    info.mem = NULL;
    info.mem_size = 0;

    if (lv_ft_font_init(&info)) {
        printf("[INFO] 成功加载字体：%s (字号：%d)\n", font_path, font_size);
        return info.font;
    } else {
        fprintf(stderr, "[ERROR] 无法加载字体：%s (字号：%d)\n", font_path, font_size);
        return NULL;
    }
}

/**
 * @brief 初始化字体缓存数组
 * @param font_cache 字体缓存数组
 * @param start_size 起始字号
 */
static void init_font_cache_array(custom_font_info_t* font_cache, int start_size)
{
    for (int i = 0; i < FONT_CACHE_SIZE; i++) {
        font_cache[i].size = start_size + i * FONT_SIZE_STEP;
        font_cache[i].font = NULL;
    }
}

/**
 * @brief 批量加载字体到缓存
 * @param font_cache 字体缓存数组
 * @param font_path 字体文件路径
 */
static void load_fonts_to_cache(custom_font_info_t* font_cache, const char* font_path)
{
    if (!font_path || !file_exists(font_path)) {
        fprintf(stderr, "[ERROR] 跳过加载不存在的字体：%s\n", font_path);
        return;
    }

    for (int i = 0; i < FONT_CACHE_SIZE; i++) {
        font_cache[i].font = load_font(font_path, font_cache[i].size);
    }
}

/**
 * @brief 打印字体加载统计信息
 * @param font_cache 字体缓存数组
 * @param font_name 字体名称
 */
static void print_font_stats(const custom_font_info_t* font_cache, const char* font_name)
{
    int loaded_count = 0;
    for (int i = 0; i < FONT_CACHE_SIZE; i++) {
        if (font_cache[i].font != NULL) {
            loaded_count++;
        }
    }
    printf("[INFO] %s 加载完成：%d/%d\n", font_name, loaded_count, FONT_CACHE_SIZE);
}

void custom_font_init(void)
{
    /* 初始化时切换到项目根目录，确保资源路径正确 */
    change_to_project_root();
    
    printf("[INFO] 开始初始化 FreeType 字体...\n");
    
    /* 初始化字体缓存数组 */
    init_font_cache_array(chinese_font_cache, FONT_SIZE_MIN);
    init_font_cache_array(english_font_cache, FONT_SIZE_MIN);
    
    /* 加载中文字体 */
    load_fonts_to_cache(chinese_font_cache, FONT_CHINESE_PATH);
    print_font_stats(chinese_font_cache, "中文字体");
    
    /* 加载英文字体 */
    load_fonts_to_cache(english_font_cache, FONT_ENGLISH_PATH);
    print_font_stats(english_font_cache, "英文字体");
    
    printf("[INFO] 字体初始化完成\n");
}

lv_font_t* get_custom_font(int font_size, int language_index)
{
    custom_font_info_t* font_cache = NULL;
    const char* font_type = NULL;
    
    /* 根据语言索引选择字体缓存 */
    switch (language_index) {
        case 0:
            font_cache = chinese_font_cache;
            font_type = "中文";
            break;
        case 1:
            font_cache = english_font_cache;
            font_type = "英文";
            break;
        default:
            fprintf(stderr, "[WARN] 无效的语言索引：%d，使用默认字体\n", language_index);
            return (lv_font_t*)&lv_font_montserrat_20;
    }
    
    /* 在缓存中查找匹配的字体 */
    for (int i = 0; i < FONT_CACHE_SIZE; i++) {
        if (font_cache[i].size == font_size && font_cache[i].font != NULL) {
            return font_cache[i].font;
        }
    }
    
    /* 未找到匹配字体时的处理策略 */
    if (font_size <= FONT_SIZE_MIN) {
        /* 小于最小字号，返回最小字号字体 */
        if (font_cache[0].font != NULL) {
            return font_cache[0].font;
        }
    } else if (font_size >= FONT_SIZE_MAX) {
        /* 大于最大字号，返回最大字号字体 */
        if (font_cache[FONT_CACHE_SIZE - 1].font != NULL) {
            return font_cache[FONT_CACHE_SIZE - 1].font;
        }
    }
    
    /* 完全找不到时使用默认字体 */
    fprintf(stderr, "[WARN] 未找到%s字体 (字号：%d)，使用默认字体\n", font_type, font_size);
    return (lv_font_t*)&lv_font_montserrat_20;
}
