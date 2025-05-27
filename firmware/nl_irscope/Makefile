all : flash

TARGET:=nl_irscope
TARGET_MCU:=CH32V003
CH32FUN=../ch32fun/ch32fun
MINICHLINK=../ch32fun/minichlink
#LINKER_SCRIPT=./ch32fun.ld

include $(CH32FUN)/ch32fun.mk

flash : cv_flash
clean : cv_clean

