###############################################################################
## compiler-opts.mak - compiler definitions and options for building the OSAL
##
## Target: m5235BCC Coldfire board with RTEMS and CEXP dynamic loader
##
## Modifications:
##
###############################################################################
## 
## Warning Level Configuration
##
## WARNINGS=-Wall -ansi -pedantic -Wstrict-prototypes
WARNINGS	= -Wall 

FIXED_HRP2_BASE =$(OSAL_SRC)/../../../toppers/hrp2_full_extension

HRP2INCDIR = $(FIXED_HRP2_BASE) \
$(FIXED_HRP2_BASE)/include \
$(FIXED_HRP2_BASE)/arch \
$(FIXED_HRP2_BASE)/arch/sh34_gcc \
$(FIXED_HRP2_BASE)/target/soisoc_gcc \

SYSINCS = $(HRP2INCDIR:%=-I%)

##
## Target Defines for the OS, Hardware Arch, etc..
##
##TARGET_DEFS = -D_RTEMS_OS_ -D_m68k_ -D$(OS) -DM5235BCC -D_EMBED_
TARGET_DEFS = -D$(OS) -D_EMBED_ -D__SH__

## 
## Endian Defines
##
ENDIAN_DEFS=-D_EB -DENDIAN=_EB -DSOFTWARE_BIG_BIT_ORDER 

##
## Compiler Architecture Switches ( double check arch switch -m52xx, m523x etc.. )
## 
##ARCH_OPTS = --pipe -fomit-frame-pointer -m528x  -B$(RTEMS_BSP_BASE)/m68k-rtems4.10/mcf5235/lib/ -specs bsp_specs -qrtems
ARCH_OPTS = -DOS_USE_EMBEDDED_PRINTF

APP_COPTS =   
APP_ASOPTS   =

##
## Extra Cflags for Assembly listings, etc.
##
LIST_OPTS    = -Wa,-a=$*.lis 

##
## gcc options for dependancy generation
## 
COPTS_D = $(APP_COPTS) $(ENDIAN_DEFS) $(TARGET_DEFS) $(ARCH_OPTS) $(SYSINCS) $(WARNINGS)

## 
## General gcc options that apply to compiling and dependency generation.
##
COPTS=$(LIST_OPTS) $(COPTS_D)

##
## Extra defines and switches for assembly code
##
##ASOPTS = $(APP_ASOPTS) -P -xassembler-with-cpp 
ASOPTS = $(APP_ASOPTS) -P -xassembler-with-cpp 

##---------------------------------------------------------
## Application file extention type
## This is the defined application extention.
## Known extentions: Mac OS X: .bundle, Linux: .so, RTEMS:
##   .s3r, vxWorks: .o etc.. 
##---------------------------------------------------------
##APP_EXT = nxe
APP_EXT = bin

####################################################
## Host Development System and Toolchain defintions
##
## Host OS utils
##
RM=rm -f
CP=cp

#
#  GNU開発環境のターゲットアーキテクチャの定義
#
GCC_TARGET = sh-elf
GCC_TARGET_PREFIX = $(GCC_TARGET)-

##
## Compiler tools
##
COMPILER   = $(GCC_TARGET_PREFIX)gcc
ASSEMBLER  = $(GCC_TARGET_PREFIX)gcc
LINKER	  = $(GCC_TARGET_PREFIX)ld
AR	   = $(GCC_TARGET_PREFIX)ar
NM         = $(GCC_TARGET_PREFIX)nm
OBJCPY     = $(GCC_TARGET_PREFIX)objcopy
