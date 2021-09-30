ORIGIN_DIR=$PWD
DRIVER_DIR="./driver"
FS_DIR="./fs"
#===================================
function install_driver() {
    sudo apt-get install make cmake fuse libfuse-dev 
    cd $DRIVER_DIR
    ./ddriver.sh -i
    ./ddriver.sh -h
    cd $ORIGIN_DIR
}

function build_workspace() {
    # $1: workspacename
    WORKSPACE_NAME=$1
    cd $FS_DIR
    override="Y"
    if test -e "$WORKSPACE_NAME" ; then 
        read -p "已存在工作目录，是否覆盖 ? (Y/n)" override 
        echo $override
    fi

    if [ "${override}" == "Y" ]; then
        rm -rf $WORKSPACE_NAME
    else
        exit 0
    fi
    
    mkdir $WORKSPACE_NAME
    sudo cp ./template/. $WORKSPACE_NAME -a 
    echo "生成工作路径: " $PWD/$WORKSPACE_NAME

    echo $WORKSPACE_NAME
    # 修改 CMakeLists
    read -p "请输入项目名称: " PROJECT_NAME 
    sed -i "s/PROJECT_NAME/${PROJECT_NAME}/g" $WORKSPACE_NAME/CMakeLists.txt
    # 修改 src
    mv $WORKSPACE_NAME/src/SRC.c $WORKSPACE_NAME/src/${PROJECT_NAME}.c

    C_COMPILER=$(which gcc)
    CPP_COMPILER=$(which g++)

    # Cmake配置工程
    mkdir $WORKSPACE_NAME/build
    GENERATOR=make

    if command -v ninja > /dev/null 2>&1; then
        GENERATOR=ninja
    else
        GENERATOR=make
    fi
    
    # 修改lunch.json
    sed -i "s/PROJECT_NAME/${PROJECT_NAME}/g" $WORKSPACE_NAME/.vscode/launch.json

    # 修改tasks.json
    sed -i "s/GENERATOR/${GENERATOR}/g" $WORKSPACE_NAME/.vscode/tasks.json

    echo "========================================================================"
    echo "一. 项目编译步骤如下: "
    echo "Step 1. SSH打开${FS_DIR}/${WORKSPACE_NAME}"
    echo "Step 2. 打开CMakeLists.txt文件"
    echo "Step 3. ctrl + shift + p呼出命令菜单"
    echo "Step 4. 输入CMake: Configure"
    echo "Step 5. 查看Generator (已用!!!!!标记)"
    echo "        1) 若为Unix Makefiles，请手动修改.vscode/tasks.json的command为make"
    echo "        2) 若为Ninja，请手动修改.vscode/tasks.json的command为ninja"
    echo "Step 6. 到${WORKSPACE_NAME}/src/${PROJECT_NAME}.c目录，打断点"
    echo "Step 7. 按下F5进行调试"
    echo ""
    echo "二. 驱动使用说明: "
    echo "我们已经为同学们安装好了ddriver (disk driver)，支持命令如下"
    echo "ddriver [options]"
    echo "options:"
    echo "-i            安装ddriver"
    echo "-t            测试ddriver[请忽略]"
    echo "-d            导出ddriver至当前工作目录[PWD]"
    echo "-r            擦除ddriver"
    echo "-l            显示ddriver的Log"
    echo "-h            打印本帮助菜单"
    echo "========================================================================"
    
    cd $ORIGIN_DIR
}

function main() {
    # $0: workspacename
    install_driver
    build_workspace $1
}

if [[ $# < 1 ]]; then
    read -p "请输入工作目录名称 (名称，非路径): " WORKSPACE_NAME 
else 
    WORKSPACE_NAME=$1
fi

main $WORKSPACE_NAME

