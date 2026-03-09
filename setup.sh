#!/bin/bash

# LVGL学习环境设置脚本

echo "开始设置LVGL学习环境..."

# 检查是否安装了git
if ! command -v git &> /dev/null; then
    echo "错误: 未找到git，请先安装git"
    exit 1
fi

# 检查是否安装了cmake
if ! command -v cmake &> /dev/null; then
    echo "错误: 未找到cmake，请先安装cmake"
    exit 1
fi

# 检查是否安装了SDL2
if ! pkg-config --exists sdl2; then
    echo "警告: 未找到SDL2，正在尝试安装..."

    # 检测操作系统并安装SDL2
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command -v apt-get &> /dev/null; then
            sudo apt-get update
            sudo apt-get install -y libsdl2-dev
        elif command -v yum &> /dev/null; then
            sudo yum install -y SDL2-devel
        else
            echo "错误: 无法自动安装SDL2，请手动安装"
            exit 1
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        if command -v brew &> /dev/null; then
            brew install sdl2
        else
            echo "错误: 未找到Homebrew，请手动安装SDL2"
            exit 1
        fi
    else
        echo "错误: 不支持的操作系统"
        exit 1
    fi
fi

# 克隆LVGL库
if [ ! -d "lvgl" ]; then
    echo "正在克隆LVGL库..."
    git clone --branch v8.3.11 --depth 1 https://github.com/lvgl/lvgl.git
else
    echo "LVGL目录已存在，跳过克隆"
fi

# 创建构建目录
mkdir -p build
cd build

# 配置和构建项目
echo "正在配置项目..."
cmake ..

echo "正在编译项目..."
make

echo ""
echo "设置完成！"
echo "运行以下命令启动LVGL模拟器:"
echo "  cd build"
echo "  ./lvgl_sim"
