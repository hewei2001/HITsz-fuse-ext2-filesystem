#!/bin/bash
ORIGIN_WORK_DIR="$PWD"

KERNEL_DDRIVER="./kernel_ddriver"
KERNEL_DEV_PATH="/dev/ddriver"

USER_DDRIVER="./user_ddriver"
USER_LOG_PATH="$HOME/ddriver_log"
USER_DEV_PATH="$HOME/ddriver"


if [ -L "$0" ]; then
    REAL_PATH=$(readlink -f "$0")
    WORK_DIR=$(cd `dirname $REAL_PATH`; pwd)
else 
    WORK_DIR=$(cd `dirname $0`; pwd)
fi

cd "$WORK_DIR" || exit

CONFIG_BLOCK_SZ=512
BLOCK_COUNT=8192


function usage(){
    echo '''
====================================================================
]]]]]]]]    ]]]]]]]]             
]      ]]   ]      ]]   ]   ]]   ]]   ]]    ]]    ]-------]   ]   ]]   
]       ]   ]       ]   ] ]]     ]]    ]]  ]]     ]_______/   ] ]]     
]      ]]   ]      ]]   ]]       ]]     ]  ]      \           ]]      
]]]]]]]]    ]]]]]]]]    ]]       ]]      ]]        ]]]]]]]]   ]]  
====================================================================
    '''
    echo "用法: ddriver [options]"
    echo "options: "
    echo "-i [k|u]      安装ddriver: [k] - kernel / [u] - user"
    echo "-t            测试ddriver[请忽略]"
    echo "-d            导出ddriver至当前工作目录[PWD]"
    echo "-r            擦除ddriver"
    echo "-l            显示ddriver的Log"
    echo "-v            显示ddriver的类型[内核模块 / 用户静态链接库]"
    echo "-h            打印本帮助菜单"
    echo "===================================================================="
}

function restore_bashrc() {
    cp "$HOME"/.bashrc_copy "$HOME"/.bashrc -f  
}

function root_permission_check() {
    current_user=$(whoami)
    if [ "$current_user" != "root" ]; then
        has_root_permission=$(sudo -l -U "$current_user" | grep "(root) ALL")
        if [ -n "$has_root_permission" ]; then
            :       # Do Nothing
        else
            echo "你没有权限安装内核模块"
            exit 1
        fi
    fi
}

function install() {
    DDRIVER_TYPE=$1
    echo "$DDRIVER_TYPE"

    if [ ! -f "$HOME"/.bashrc_copy ]; then
        cp "$HOME"/.bashrc "$HOME"/.bashrc_copy
    fi

    restore_bashrc
    
    if [ "$DDRIVER_TYPE" == "k" ]; then   
        
        root_permission_check

        cd $KERNEL_DDRIVER || exit
        make -f ./Makefile 
        sudo rm $KERNEL_DEV_PATH>/dev/null 2>&1 
        sudo rmmod ddriver>/dev/null 2>&1 
        sudo dmesg -C
        sudo insmod ./ddriver.ko
        in=$(dmesg | tail -n 1)
        tokens=("$in")
        major_number=${tokens[${#tokens[*]}-1]}
        echo Major Number: "$major_number"
        sudo mknod $KERNEL_DEV_PATH c "$major_number" 0
        sudo chmod 777 $KERNEL_DEV_PATH
        sudo rm /usr/bin/ddriver>/dev/null 2>&1
        sudo ln -s "$WORK_DIR"/ddriver.sh /usr/bin/ddriver>/dev/null 2>&1
        echo "" >>"$HOME"/.bashrc
        source "$HOME"/.bashrc   

        echo "export DDRIVER_TYPE='k'" >>"$HOME"/.bashrc
        source "$HOME"/.bashrc
        cd ..
    else 
        touch -f "$USER_DEV_PATH"
        
        LAST_DIR=$PWD
        cd $USER_DDRIVER || exit
        make all -f ./Makefile
        
        mkdir -p bin
        
        echo "" >>"$HOME"/.bashrc
        source "$HOME"/.bashrc   
        
        echo "export PATH=$PWD/bin:$PATH" >>"$HOME"/.bashrc  
        source "$HOME"/.bashrc     
        
        echo "export DDRIVER_TYPE='u'" >>"$HOME"/.bashrc
        source "$HOME"/.bashrc

        cd "$LAST_DIR" || exit

        rm "$LAST_DIR"/user_ddriver/bin/ddriver>/dev/null 2>&1
        ln -s "$LAST_DIR"/ddriver.sh "$LAST_DIR"/user_ddriver/bin/ddriver>/dev/null 2>&1
    fi
}

function test(){
    if [ "$DDRIVER_TYPE" == "k" ]; then   
        # test read
        sudo dd if=$KERNEL_DEV_PATH of=read1 bs=$CONFIG_BLOCK_SZ count=$BLOCK_COUNT
        # test write
        sudo dd if=/dev/random of=$KERNEL_DEV_PATH bs=$CONFIG_BLOCK_SZ count=2
        # test read
        sudo dd if=$KERNEL_DEV_PATH of=read2 bs=$CONFIG_BLOCK_SZ count=$BLOCK_COUNT
    else 
        exit
    fi
}

function log() {
    if [ "$DDRIVER_TYPE" == "k" ]; then  
        dmesg | grep ddriver
    else 
        cat "$USER_LOG_PATH"
    fi
}

function dump(){
    sudo rm "$ORIGIN_WORK_DIR"/ddriver_dump>/dev/null 2>&1 
    if [ "$DDRIVER_TYPE" == "k" ]; then  
        echo "目标设备 $KERNEL_DEV_PATH"
        sudo dd if=$KERNEL_DEV_PATH of="$ORIGIN_WORK_DIR"/ddriver_dump bs=$CONFIG_BLOCK_SZ count=$BLOCK_COUNT
    else 
        echo "目标设备 $USER_DEV_PATH"
        dd if="$USER_DEV_PATH" of="$ORIGIN_WORK_DIR"/ddriver_dump bs=$CONFIG_BLOCK_SZ count=$BLOCK_COUNT
    fi
    echo "文件已导出至$ORIGIN_WORK_DIR/ddriver_dump，请安装HexEditor插件查看其内容"
}

function clean(){
    if [ "$DDRIVER_TYPE" == "k" ]; then  
        echo "目标设备 $KERNEL_DEV_PATH"
        sudo dd if=/dev/zero of=$KERNEL_DEV_PATH bs=$CONFIG_BLOCK_SZ count=$BLOCK_COUNT
    else
        echo "目标设备 $USER_DEV_PATH"
        dd if=/dev/zero of="$USER_DEV_PATH" bs=$CONFIG_BLOCK_SZ count=$BLOCK_COUNT
    fi 
}

function version () {
    if [ "$DDRIVER_TYPE" == "k" ]; then  
        echo "内核设备: $KERNEL_DEV_PATH"
    else
        echo "静态链接库设备: $USER_DEV_PATH"
    fi 
}

if [ $# == 0 ]; then
    usage
else 
    while getopts 'i:tdhrlv' OPT; do
        case $OPT in
            i) install "$OPTARG"
            ;;
            t) test
            ;;
            d) dump
            ;;
            r) clean
            ;;
            l) log
            ;;
            v) version 
            ;;
            h) usage
            ;;
            ?) usage
            ;;
        esac
    done
fi

cd "$ORIGIN_WORK_DIR" || exit