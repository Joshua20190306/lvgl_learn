# LVGL学习项目

## 项目介绍
这是一个用于学习 LVGL (Light and Versatile Graphics Library) v8.3.11 的基础项目，使用 SDL2 作为后端在桌面环境中模拟嵌入式 GUI 应用。项目提供了完整的 LVGL 模拟器环境，支持 FreeType 字体渲染、多种 UI 组件演示和中文显示功能。

### 核心特性
- ✅ **SDL2 窗口化模拟** - 在 PC 上运行 LVGL 应用，无需硬件
- ✅ **FreeType 字体引擎** - 支持 TTF 字体动态加载（中英文）
- ✅ **完整 UI 组件库** - 按钮、滑块、开关、文本框等常用控件
- ✅ **中文显示支持** - 内置文泉驿字体和系统 CJK 字体
- ✅ **实时帧率显示** - FPS 监控性能表现
- ✅ **事件驱动架构** - 基于 LVGL 的事件处理机制
- ✅ **CMake 构建系统** - 跨平台编译管理

## 快速开始

### 1. 克隆仓库
```bash
git clone --recurse-submodules https://github.com/your-username/lvgl_learn.git
cd lvgl_learn
```

### 2. 初始化子模块（如果 lvgl 目录为空）
```bash
git submodule update --init --recursive
```

### 3. 安装依赖
**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential cmake libsdl2-dev libfreetype6-dev pkg-config
```

**CentOS/RHEL/Fedora:**
```bash
# CentOS/RHEL
sudo yum install gcc cmake SDL2-devel freetype-devel

# Fedora
sudo dnf install gcc cmake SDL2-devel freetype-devel
```

**macOS:**
```bash
brew install cmake sdl2 freetype
```

### 4. 构建并运行
```bash
./setup.sh
```

或手动构建:
```bash
mkdir build && cd build
cmake ..
make
./lvgl_sim
```

## 项目结构

```
lvgl_learn/
├── lvgl/                    # LVGL库源码 (v8.3.11, Git 子模块)
├── src/                     # 项目源代码
│   ├── main.c              # 主程序入口
│   ├── sdl_monitor.c/h     # SDL2 显示驱动（帧缓冲、鼠标输入）
│   ├── lvgl_port.c/h       # LVGL 端口适配层（显示/输入驱动注册）
│   ├── ui_elements.c/h     # UI 元素创建和事件处理
│   ├── freetype_example.c/h # FreeType 字体管理（动态 TTF 加载）
│   ├── lv_font_wqy_14.c    # 文泉驿 14px 中文字体（内嵌）
│   ├── RED.c               # 红色图片资源（内嵌）
│   └── lvgl_sim.h          # 公共头文件
├── lv_conf.h               # LVGL 配置文件
├── CMakeLists.txt          # CMake 构建配置
├── setup.sh                # 自动化设置脚本
└── README.md               # 项目文档
```

## 技术规格

### 显示配置
- **分辨率**: 480×480 像素
- **颜色深度**: 32 位 (ARGB8888)
- **刷新周期**: 30ms
- **渲染模式**: LV_DISPLAY_RENDER_MODE_DIRECT
- **帧缓冲**: 双缓冲 (LV_SDL_BUF_COUNT = 2)

### 内存配置
- **LVGL 内存池**: 64KB
- **FreeType 缓存**: 16MB
- **Tick 周期**: 5ms

### 字体支持
| 字体类型 | 名称 | 大小 | 说明 |
|---------|------|------|------|
| 内置英文 | Montserrat 14/20 | 固定 | LVGL 自带 |
| 内嵌中文 | 文泉驿 14px | ~13.8MB | 点阵字体 |
| 系统中文 | SimSun 16px CJK | 固定 | 1000 个常用汉字 |
| TTF 中文 | 文泉驿比例字体 | 8-26px | FreeType 动态渲染 |
| TTF 英文 | Courier | 8-26px | FreeType 动态渲染 |

### UI 组件演示
项目演示了以下 LVGL 组件：
- 📝 **Label** - 文本标签（多字体、多语言）
- 🔘 **Button** - 按钮（带点击事件）
- 🎚️ **Slider** - 滑块（值变化监听）
- 🔛 **Switch** - 开关控件
- 📝 **Textarea** - 文本输入框（带焦点管理）
- 🖼️ **Image** - 图片显示（内嵌资源）
- ⏱️ **Timer** - 软件定时器（标题闪烁效果）
- 📊 **FPS Counter** - 实时帧率显示

## 子模块注意事项

⚠️ **重要提示：**

1. **首次克隆时**推荐使用 `--recurse-submodules` 参数，可自动初始化子模块

2. **如果忘记加参数**，执行以下命令初始化：
   ```bash
   git submodule update --init --recursive
   ```

3. **更新 LVGL 库**到最新版本：
   ```bash
   git submodule update --remote
   ```

4. **配置 Git 用户信息**（必需，否则合并操作会失败）：
   ```bash
   git config --global user.name "Your Name"
   git config --global user.email "your.email@example.com"
   ```

5. **加速子模块克隆**（可选）：
   ```bash
   git config --global submodule.fetchJobs 8
   ```

## 开发指南

### 添加新的 UI 元素
在 [`ui_elements.c`](src/ui_elements.c) 中的 `create_ui_elements()` 函数中添加：

```c
// 1. 创建对象
lv_obj_t *new_obj = lv_xxx_create(lv_scr_act());

// 2. 设置样式和属性
lv_obj_set_size(new_obj, 100, 50);
lv_obj_align(new_obj, LV_ALIGN_CENTER, 0, 0);

// 3. 添加事件回调
lv_obj_add_event_cb(new_obj, your_event_cb, LV_EVENT_CLICKED, NULL);
```

### 使用自定义字体
```c
#include "freetype_example.h"

// 获取 16px 中文字体
lv_font_t* font = get_custom_font(16, 0);  // language_index: 0=中文，1=英文

// 应用到标签
lv_obj_set_style_text_font(label, font, 0);
```

### 修改屏幕分辨率
编辑 [`lvgl_sim.h`](src/lvgl_sim.h):
```c
#define MONITOR_HOR_RES 800  // 宽度
#define MONITOR_VER_RES 600  // 高度
```

### 调整 LVGL 配置
编辑 [`lv_conf.h`](lv_conf.h)，常见配置项：
```c
#define LV_COLOR_DEPTH 32        // 颜色深度
#define LV_MEM_SIZE (64U * 1024U) // 内存大小
#define LV_DEF_REFR_PERIOD 30    // 刷新周期
#define LV_USE_FREETYPE 1        // FreeType 支持
```

## 常见问题

### Q1: 编译时找不到 SDL2
**解决**: 确保已安装 SDL2 开发库：
```bash
# Ubuntu/Debian
sudo apt install libsdl2-dev

# 验证安装
pkg-config --modversion sdl2
```

### Q2: FreeType 相关错误
**解决**: 安装 FreeType 开发库：
```bash
sudo apt install libfreetype6-dev pkg-config
```

### Q3: 运行时字体文件不存在
**解决**: 确保在项目根目录下运行程序，或使用绝对路径：
```bash
# 正确：在项目根目录运行
cd /path/to/lvgl_learn
./build/lvgl_sim

# 或检查字体文件路径
ls res/font/
```

### Q4: 窗口显示异常或崩溃
**解决**: 
- 检查 SDL2 是否正确初始化
- 确认显示器分辨率足够（至少 480×480）
- 尝试关闭其他占用图形资源的应用

### Q5: 子模块更新失败
**解决**: 
```bash
# 检查 Git 用户信息
git config user.name
git config user.email

# 如未设置，按前述方法配置后重试
```

## 学习资源

### 官方文档
- [LVGL 官方文档](https://docs.lvgl.io/) - 完整的 API 参考和教程
- [LVGL GitHub](https://github.com/lvgl/lvgl) - 源码和 Issue 追踪
- [SDL2 文档](https://wiki.libsdl.org/) - SDL2 API 参考

### 示例代码
- `src/main.c` - 主循环和初始化流程
- `src/ui_elements.c` - UI 组件创建示例
- `src/freetype_example.c` - FreeType 字体使用示例
- `lvgl/examples/` - LVGL 官方示例集合

### 进阶主题
- 自定义 Widget 开发
- 多线程渲染优化
- 内存管理和性能调优
- 移植到嵌入式平台（STM32、ESP32 等）

## 项目扩展建议

### 初级练习
1. 修改按钮点击计数器的初始值
2. 改变滑块的颜色和大小
3. 添加新的 Label 显示不同文本
4. 调整 FPS 显示位置

### 中级练习
1. 创建 TabView 实现多页面切换
2. 使用 Chart 绘制实时数据曲线
3. 添加 Keyboard 实现虚拟键盘输入
4. 实现自定义主题（Theme）

### 高级练习
1. 集成触摸屏校准功能
2. 实现动画过渡效果
3. 添加文件系统支持（FATFS）
4. 移植到真实硬件平台

## 贡献与反馈

欢迎提交 Issue 和 Pull Request！
- Bug 报告和功能建议请提交 Issue
- 代码改进和新功能请提交 PR
- 文档错别字和格式问题也欢迎指正

## 许可证

本项目使用的 LVGL库遵循 MIT 许可证。
SDL2 和 FreeType 遵循各自的开源许可证。

---

**祝您学习愉快！** 🎉
