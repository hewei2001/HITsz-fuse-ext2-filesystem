#!/bin/bash
ORIGIN_WORK_DIR=$PWD
WORK_DIR=$(cd `dirname $0`; pwd)
cd $WORK_DIR

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
    echo "-i            安装ddriver"
    echo "-t            测试ddriver[请忽略]"
    echo "-d            导出ddriver至当前工作目录[PWD]"
    echo "-r            擦除ddriver"
    echo "-h            打印本帮助菜单"
    echo "-e            为当前目录配置工作环境"
}

function install() {
    make -f ./Makefile 
    sudo rm /dev/ddriver>/dev/null 2>&1 
    sudo rmmod ddriver>/dev/null 2>&1 
    sudo dmesg -C
    sudo insmod ./ddriver.ko
    in=$(dmesg | tail -n 1)
    tokens=($in)
    major_number=${tokens[${#tokens[*]}-1]}
    echo Major Number: $major_number
    sudo mknod /dev/ddriver c $major_number 0
    sudo chmod 777 /dev/ddriver
}

function test(){
    # test read
    sudo dd if=/dev/ddriver of=read1 bs=$CONFIG_BLOCK_SZ count=$BLOCK_COUNT
    # test write
    sudo dd if=/dev/random of=/dev/ddriver bs=$CONFIG_BLOCK_SZ count=2
    # test read
    sudo dd if=/dev/ddriver of=read2 bs=$CONFIG_BLOCK_SZ count=$BLOCK_COUNT
}

function dump(){
    sudo rm $ORIGIN_WORK_DIR/ddriver_dump>/dev/null 2>&1 
    sudo dd if=/dev/ddriver of=$ORIGIN_WORK_DIR/ddriver_dump bs=$CONFIG_BLOCK_SZ count=$BLOCK_COUNT
    echo "文件已导出至$ORIGIN_WORK_DIR/ddriver_dump，请安装HexEditor插件查看其内容"
}

function clean(){
    sudo dd if=/dev/zero of=/dev/ddriver bs=$CONFIG_BLOCK_SZ count=$BLOCK_COUNT
}

function env() {
    echo "TODO"
}

if [ $# == 0 ]; then
    usage
else 
    while getopts 'itdher' OPT; do
        case $OPT in
            i) install
            ;;
            t) test
            ;;
            d) dump
            ;;
            e) env 
            ;;
            r) clean
            ;;
            h) usage
            ;;
            ?) usage
            ;;
        esac
    done
fi
cd $ORIGIN_WORK_DIR