#!/bin/bash

ROOT_DIR="$PWD"
DRIVER_DIR="./driver"
FS_DIR="./fs"

#===================================
function mkdev() {
    touch -f "$HOME/ddriver"
}

function install_driver() {
    if command -v apt-get > /dev/null 2>&1; then
        sudo apt-get install make cmake fuse libfuse-dev
    else 
        sudo yum install make cmake fuse fuse-devel 
    fi 
    
    cd $DRIVER_DIR || exit
    ./ddriver.sh -i u           
    ./ddriver.sh -h
    cd "$ROOT_DIR" || exit
}

function build_workspace() {
    # $1: workspacename
    WORKSPACE_NAME="$1"
    cd $FS_DIR || exit
    override="Y"
    if test -e "$WORKSPACE_NAME" ; then 
        read -r -p "已存在工作目录，是否覆盖 ? (Y/n)" override 
        echo "$override"
    fi

    if [ "$override" == "Y" ]; then
        rm -rf "$WORKSPACE_NAME"
    else
        exit 0
    fi
    
    mkdir "$WORKSPACE_NAME"
    sudo cp ./template/. "$WORKSPACE_NAME" -a 
    echo "生成工作路径: " "$PWD"/"$WORKSPACE_NAME"
    # 修改 CMakeLists
    read -r -p "请输入项目名称: " PROJECT_NAME 
    sed -i "s/PROJECT_NAME/${PROJECT_NAME}/g" "$WORKSPACE_NAME"/CMakeLists.txt
    # 修改 src
    mv "$WORKSPACE_NAME"/src/SRC.c "$WORKSPACE_NAME"/src/"$PROJECT_NAME".c

    # 修改测试脚本
    sed -i "s/SAMPLE_PROJECT_NAME/${PROJECT_NAME}/g" "$WORKSPACE_NAME"/tests/fs_test.sh
    
    # C_COMPILER=$(which gcc)
    # CPP_COMPILER=$(which g++)

    # Cmake配置工程
    mkdir "$WORKSPACE_NAME"/build
    GENERATOR="make"

    if command -v ninja > /dev/null 2>&1; then
        GENERATOR="ninja"
    else
        GENERATOR="make"
    fi
    
    # 修改lunch.json
    sed -i "s/PROJECT_NAME/${PROJECT_NAME}/g" "$WORKSPACE_NAME"/.vscode/launch.json

    # 修改tasks.json
    sed -i "s/GENERATOR/${GENERATOR}/g" "$WORKSPACE_NAME"/.vscode/tasks.json


    echo "
========================================================================
一. 项目编译步骤如下: 
Step 1. SSH打开${FS_DIR}/${WORKSPACE_NAME}
Step 2. 打开CMakeLists.txt文件
Step 3. ctrl + shift + p呼出命令菜单
Step 4. 输入CMake: Configure
Step 5. 查看Generator (已用!!!!!标记)
        1) 若为Unix Makefiles，请手动修改.vscode/tasks.json的command为make
        2) 若为Ninja，请手动修改.vscode/tasks.json的command为ninja
Step 6. 到${WORKSPACE_NAME}/src/${PROJECT_NAME}.c目录，打断点
Step 7. 按下F5进行调试
二. 驱动使用说明: 
我们已经为同学们安装好了ddriver (disk driver)，支持命令如下
ddriver [options]
options:
-i            安装ddriver
-t            测试ddriver[请忽略]
-d            导出ddriver至当前工作目录[PWD]
-r            擦除ddriver
-l            显示ddriver的Log
-h            打印本帮助菜单
三. 文件结构说明
1. 项目根目录: ${ROOT_DIR}
|--driver           驱动文件
|--fs               FUSE文件系统文件
|--LICENSE          LICENSE GPL v3          
|--README.md        实验相关说明
|--setenv.sh        项目环境配置
2. FUSE文件系统目录: 以${ROOT_DIR}/simplefs (SFS) 为例
|--.vscode          VSCode启动配置
|--build            CMake构建目录
|--CMake            CMake Module目录
|--include          SFS文件系统头文件
|--src              SFS文件系统源文件
|--tests            测试脚本，tests/mnt为测试挂载点
|--CMakeLists.txt   -    
|--Makefile         -
|--ddriver_dump     ddriver -d后的磁盘布局，可用HexEditor查看
|--README.md        SFS FUSE开发记录
四. FUSE体验
在本项目中，我们已经有了两个FUSE文件系统样例
1. SFS  (${ROOT_DIR}/simplefs)
2. MYFS (${ROOT_DIR}/sapmles)
大家直接SSH切到相应目录，打断点运行即可
========================================================================
    " >"$ROOT_DIR"/$FS_DIR/"$WORKSPACE_NAME"/SPEC.txt
    cat "$ROOT_DIR"/$FS_DIR/"$WORKSPACE_NAME"/SPEC.txt
    echo "上述说明已生成至" "$ROOT_DIR"/fs/"$WORKSPACE_NAME"/SPEC.txt
    cd "$ROOT_DIR" || exit
}

function main() {
    # $1: workspacename
    mkdev
    install_driver
    build_workspace "$1"
}

if [[ $# -lt 1 ]]; then
    read -r -p "请输入工作目录名称 ([工作]目录将被至于./fs目录下): " WORKSPACE_NAME 
else 
    WORKSPACE_NAME="$1"
fi

main "$WORKSPACE_NAME"
