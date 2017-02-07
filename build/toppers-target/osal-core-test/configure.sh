#!/bin/sh

TOPPERS_PATH=~/Workspace/toppers/hrp2_full_extension/
TOPPERS_TARGET=soisoc_gcc
APPTARGET=osal-core-test


perl $TOPPERS_PATH/configure -T $TOPPERS_TARGET -A $APPTARGET -a $OSAL_SRC/tests/$APPTARGET
