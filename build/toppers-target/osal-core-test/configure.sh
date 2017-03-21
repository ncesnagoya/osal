#!/bin/sh

TOPPERS_PATH=../../../../hrp2
TOPPERS_TARGET=soisoc_gcc
APPTARGET=osal-core-test

perl $TOPPERS_PATH/configure -T $TOPPERS_TARGET -A $APPTARGET \
    -a "../../../../osal/src/tests/$APPTARGET \
    ../../../../osal/src/os/inc \
    ../../../../osal/src/os/toppers \
    ../../../../osal/src/bsp/soisoc-toppers/config \
    ../../../../osal/src/bsp/soisoc-toppers/src \
    ../../../../osal/src/bsp/soisoc-toppers/ut-src \
    ../../../../osal/ut_assert/inc \
    ../../../../osal/ut_assert/src " \
    -U "osapi.o ostimer.o utassert.o bsp_ut.o bsp_start.o" \
    -O -D__SH__
