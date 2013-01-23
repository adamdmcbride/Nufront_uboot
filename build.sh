#!/bin/bash

UBOOT_ROOT=$(cd $(dirname $0);pwd)     #uboot code的根目录
CONFIG_FILE=""                         #uboot配置文件
MAKE_TARGET=""                         #编译的平台
UBOOT_OUT=${UBOOT_ROOT}/../../out/

F_CLEAN=0
F_TARGET=0

function show_help() {
	echo "
usage :
    build.sh [-c] [-i] [-t <target>] [-h]

	-h:            show uboot build help.
        -c:            clear project.
	-i:            initialize code use repo/git tools.
        -t <target>:   target type:
                           full_nusmart3_pad       --> ns115_pad_ref_config
                           full_nusmart3_pad_nw51  --> ns115_pad_ref_config
                           full_nusmart3_stick     --> ns115_hdmi_stick_config
                           full_nusmart3_prototype --> ns115_pad_prototype_config
"
}

function LOGE {
    TARGET_LOG=${UBOOT_ROOT}/../../out/$MAKE_TARGET.log
	echo "[ERROR]: $1" |tee -a $TARGET_LOG
	exit 0;
}

function LOGI() {
    TARGET_LOG=${UBOOT_ROOT}/../../out/$MAKE_TARGET.log
    echo "[INFO]: $1" |tee -a $TARGET_LOG
}

function select_platform() {
    ARG=$1
	if [[ $1 = "full_nusmart3_pad" ]]; then
		MAKE_TARGET=$ARG
        CONFIG_FILE=ns115_pad_ref_config
    elif [[ $1 = "full_nusmart3_pad_nw51" ]]; then
        MAKE_TARGET=$ARG
        CONFIG_FILE=ns115_pad_ref_config
	elif [[ $1 = "full_nusmart3_stick" ]]; then
		MAKE_TARGET=$ARG
        CONFIG_FILE=ns115_hdmi_stick_config
	elif [[ $1 = "full_nusmart3_prototype" ]]; then
		MAKE_TARGET=$ARG
        CONFIG_FILE=ns115_pad_prototype_config
	else
		LOGE "compile uboot fail, not found '$1' configure."
	fi
}

function select_kernel_version() {
	echo
}

function INIT_CODE_CMD() {
    #初始化code.
	repo init -u ssh://zhengxing@192.168.2.17:29418/platform/manifest.git -b nusmart3_uboot.master
	repo sync
	repo start --all nusmart3_uboot.master
}

function check_uboot_out_dir() {
    if [[ ! -d $1 ]]; then
		mkdir -p $1
	fi
}

function compile_uboot_copy() {
    make_uboot
    UBOOT_OUT=${UBOOT_ROOT}/../../out/$MAKE_TARGET
    check_uboot_out_dir $UBOOT_OUT
    LOGI "cp ${UBOOT_ROOT}/u-boot.img ${UBOOT_OUT}"
    cp ${UBOOT_ROOT}/u-boot.img ${UBOOT_OUT}
    if [[ $? != 0 ]]; then
        LOGE "copy u-boot.img fail......"
    else
        LOGI "copy u-boot.img to $UBOOT_OUT"
    fi
}

function make_main() {
    if [ $F_CLEAN -eq 1 -a $F_TARGET -ne 1 ]; then
        LOGI "make clean uboot......"
        P1=$(pwd)
        cd $UBOOT_ROOT
        make distclean
        cd $P1
    elif [ $F_TARGET -eq 1 -a $F_CLEAN -ne 1 ]; then
        compile_uboot_copy
    elif [ $F_TARGET -eq 1 -a $F_CLEAN -eq 1 ]; then
        make_clean
    fi
}

function make_uboot() {
    LOGI "========================================"
    LOGI "compile uboot......"
    LOGI "========================================"
	LOGI "target is ${MAKE_TARGET} ......"
	LOGI "config is ${CONFIG_FILE} ......"
    P1=`pwd`
	read -t 4
	cd $UBOOT_ROOT
	make $CONFIG_FILE

	if [[ $? != 0 ]]; then
		LOGE "make $CONFIG_FILE  fail."
		exit -1;
	fi
	make -j8
	if [[ $? != 0 ]]; then
		LOGE "make uboot  fail."
		exit -1;
	fi
	./tools/add_head ./u-boot ./u-boot.img
	if [[ $? != 0 ]]; then
		LOGE "./tools/add_head execute fail."
		exit -1;
	fi
	cd $P1
}

function make_clean() {
    UBOOT_OUT=${UBOOT_ROOT}/../../out/$MAKE_TARGET
    if [[ ! -d $UBOOT_OUT ]]; then
		LOGI "$UBOOT_OUT is empty......"
    else
        LOGI "clean ${UBOOT_OUT}/uboot.img......"
        rm -fr ${UBOOT_OUT}/u-boot.img
	fi
	P1=$(pwd)
	cd $UBOOT_ROOT
    LOGI "make clean uboot......"
	make distclean
	cd $P1
}

###################################################

if [[ $# == 0 ]]; then
	show_help;
	exit 0;
fi

while getopts chik:t: OPT;
do
	case $OPT in
		t)
		    select_platform $OPTARG
            F_TARGET=1
		    ;;
		k)
		    select_kernel_version  $OPTARG
		    ;;
        c)
            F_CLEAN=1
            ;;
        h)
            show_help
            exit 0
            ;;
        i)
            #INIT_CODE_CMD
            exit 0
            ;;
		*)
		    show_help
		    exit 1
		    ;;
	esac
done

make_main
