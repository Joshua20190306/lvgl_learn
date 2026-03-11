# LVGL 学习项目

## 项目介绍
这是一个用于学习LVGL (Light and Versatile Graphics Library) 的基础项目，使用SDL2作为后端在桌面环境中模拟嵌入式GUI应用。

## 目录结构
- `lvgl/`: LVGL库源码
- `lv_conf.h`: LVGL配置文件
- `src/`: 示例代码
- `CMakeLists.txt`: CMake构建文件
- `setup.sh`: 环境自动设置脚本

## 环境要求
- C编译器 (GCC/Clang)
- CMake (3.10+)
- SDL2 开发库 (用于模拟器)

### Ubuntu/Debian 系统安装依赖
```bash
sudo apt update
sudo apt install build-essential cmake libsdl2-dev
```

### CentOS/RHEL/Fedora 系统安装依赖
```bash
# CentOS/RHEL
sudo yum install gcc cmake SDL2-devel

# Fedora
sudo dnf install gcc cmake SDL2-devel
```

### macOS 系统安装依赖
```bash
# 使用 Homebrew
brew install cmake sdl2

# 使用 MacPorts
sudo port install cmake libsdl2
```

## 构建步骤

### 方法一：使用 setup.sh 脚本（推荐）
```bash
# 使脚本可执行
chmod +x setup.sh

# 运行设置脚本（将自动安装依赖、克隆LVGL、构建项目）
./setup.sh
```

### 方法二：手动构建
```bash
# 创建构建目录
mkdir build
cd build

# 配置项目
cmake ..

# 编译项目
make

# 运行程序
./lvgl_sim
```

## 项目配置
- 默认屏幕尺寸：480x480像素
- 颜色深度：16位（RGB565）
- 刷新周期：5毫秒
- 内存池：根据[lv_conf.h](file:///home/hdl/work/lv_conf.h)配置

## 代码结构
- [main.c](file:///home/hdl/work/src/main.c) 包含显示驱动([flush_cb](file:///home/hdl/work/src/main.c#L31-L50))和输入驱动([mouse_read_cb](file:///home/hdl/work/src/main.c#L53-L59))的实现
- 显示驱动负责将LVGL的颜色数据转换为SDL2纹理
- 输入驱动将SDL2鼠标事件转换为LVGL输入设备数据
- 项目包含按钮、滑块、开关等基础UI组件示例

## 学习资源
- LVGL官方文档: https://docs.lvgl.io/
- LVGL GitHub: https://github.com/lvgl/lvgl
- SDL2官方文档: https://wiki.libsdl.org/