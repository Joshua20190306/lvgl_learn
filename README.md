# LVGL 学习项目

## 项目介绍
这是一个用于学习LVGL (Light and Versatile Graphics Library) 的基础项目。

## 目录结构
- `lvgl/`: LVGL库源码
- `lv_conf.h`: LVGL配置文件
- `src/`: 示例代码
- `CMakeLists.txt`: CMake构建文件

## 环境要求
- C编译器 (GCC/Clang)
- CMake (3.10+)
- SDL2 (用于模拟器)

## 构建步骤
```bash
mkdir build
cd build
cmake ..
make
./lvgl_sim
```

## 学习资源
- LVGL官方文档: https://docs.lvgl.io/
- LVGL GitHub: https://github.com/lvgl/lvgl
