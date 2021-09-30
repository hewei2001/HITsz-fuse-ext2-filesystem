#!/bin/bash
ORIGIN_WORK_DIR=$PWD

WORK_DIR=$(cd `dirname $0`; pwd)
cd $WORK_DIR

MNTPOINT='./mnt'

function test_touch() {

}

function test_mkdir() {

}

function test_ls() {

}

function test_mv() {

}

function test_rm() {

}

function test_mnt() {
    
}

function test_main() {
    test_touch
    test_mkdir
    test_ls
}

cd $ORIGIN_WORK_DIR