#!/bin/bash
ORIGIN_WORK_DIR=$PWD

WORK_DIR=$(cd `dirname $0`; pwd)
cd $WORK_DIR

MNTPOINT='./mnt'
PROJECT_NAME="nfs"
ALL_POINTS=23
POINTS=0

function pass() {
    RES=$1
    POINTS=$(($POINTS+1))
    echo -e "\033[32mpass: ${RES}\033[0m"
}

function fail() {
    RES=$1
    echo -e "\033[31mfail: ${RES}\033[0m"
}

function test_mount() {
    TEST_CASE=$1
    echo ">>>>>>>>>>>>>>>>>>>> TEST_MOUNT"
    ../build/${PROJECT_NAME} --device="$HOME"/ddriver ${MNTPOINT}
    if [ $? -ne 0 ]; then
        fail $TEST_CASE
        exit 1
    else    
        pass $TEST_CASE
    fi
    
    echo "<<<<<<<<<<<<<<<<<<<<"
}

function core_tester() {
    CMD=$1
    PARAM=$2
    echo "TEST: "$CMD $PARAM
    $CMD $PARAM
    if [ $? -ne 0 ]; then
        fail $CMD $PARAM
    else     
        pass "-> $CMD $PARAM"
    fi
}

function test_mkdir() {
    TEST_CASE=$1
    echo ">>>>>>>>>>>>>>>>>>>> TEST_MKDIR"
    
    core_tester mkdir ${MNTPOINT}/dir0
    core_tester mkdir ${MNTPOINT}/dir0/dir0
    core_tester mkdir ${MNTPOINT}/dir0/dir0/dir0
    core_tester mkdir ${MNTPOINT}/dir1

    
    echo "<<<<<<<<<<<<<<<<<<<<"
}

function test_touch() {
    TEST_CASE=$1
    echo ">>>>>>>>>>>>>>>>>>>> TEST_TOUCH"
    
    core_tester touch ${MNTPOINT}/file0;
    core_tester touch ${MNTPOINT}/dir0/file0;
    core_tester touch ${MNTPOINT}/dir0/dir0/file0;
    core_tester touch ${MNTPOINT}/dir0/dir0/dir0/file0;
    core_tester touch ${MNTPOINT}/dir1/file0;

    
    echo "<<<<<<<<<<<<<<<<<<<<"
}

function test_ls() {
    TEST_CASE=$1
    echo ">>>>>>>>>>>>>>>>>>>> TEST_LS"
    
    core_tester ls ${MNTPOINT}/;
    core_tester ls ${MNTPOINT}/dir0;
    core_tester ls ${MNTPOINT}/dir0/dir0;
    core_tester ls ${MNTPOINT}/dir0/dir0/dir0;
    core_tester ls ${MNTPOINT}/dir1;

    echo "<<<<<<<<<<<<<<<<<<<<"
}

function test_cp() {
    TEST_CASE=$1
    echo ">>>>>>>>>>>>>>>>>>>> TEST_CP"
    
    cp ${MNTPOINT}/file0 ${MNTPOINT}/file1 
    if [ $? -ne 0 ]; then
        fail $TEST_CASE
    fi

    pass $TEST_CASE
    
    echo "<<<<<<<<<<<<<<<<<<<<"
}

function test_remount() {
    TEST_CASE=$1
    echo ">>>>>>>>>>>>>>>>>>>> TEST_REMOUNT"
    
    fusermount -u ${MNTPOINT}
    if [ $? -ne 0 ]; then
        fail "umount"
    else 
        pass "-> fusermount -u ${MNTPOINT}"
    fi

    ../build/${PROJECT_NAME} --device="$HOME"/ddriver ${MNTPOINT}
    if [ $? -ne 0 ]; then
        fail "remount"
    else 
        pass "-> ../build/${PROJECT_NAME} --device="$HOME"/ddriver ${MNTPOINT}"
    fi
    
    core_tester ls ${MNTPOINT}/;
    core_tester ls ${MNTPOINT}/dir0;
    core_tester ls ${MNTPOINT}/dir0/dir0;
    core_tester ls ${MNTPOINT}/dir0/dir0/dir0;
    core_tester ls ${MNTPOINT}/dir1;

    sleep 1
    
    fusermount -u ${MNTPOINT}
    if [ $? -ne 0 ]; then
        fail "umount finally"
    else 
        pass "-> fusermount -u ${MNTPOINT}"
    fi

    echo "<<<<<<<<<<<<<<<<<<<<"
}


function test_main() {
    ddriver -r
    test_mount "[all-the-mount-test]"
    echo ""
    test_mkdir "[all-the-mkdir-test]"
    echo ""
    test_touch "[all-the-mkdir-test]"
    echo ""
    test_ls "[all-the-ls-test]"
    echo ""
    test_remount "[all-the-remount-test]"
    echo ""

    if [ $POINTS -eq $ALL_POINTS ]; then
        pass "恭喜你，通过所有测试 ($ALL_POINTS/$ALL_POINTS)"
    else 
        fail "再接再厉! ($POINTS/$ALL_POINTS)"
    fi
}

test_main
cd $ORIGIN_WORK_DIR