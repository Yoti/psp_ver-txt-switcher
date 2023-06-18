TARGET = homebrew
OBJS = main.o

INCDIR = ./inc
CFLAGS = -O2 -G0 -Wall
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

LIBDIR = ./lib
LIBS = ./lib/libpspdebug_ru.a
LDFLAGS =

EXTRA_TARGETS = EBOOT.PBP
PSP_FW_VERSION = 360
BUILD_PRX = 1

PSP_EBOOT_TITLE = VERSiON.TXT SWiTCHER
PSP_EBOOT_ICON = ICON0.PNG

ifeq ($(OFW_MODE), 1)
CFLAGS += -DOFW_MODE=1
endif

PSPSDK=$(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build.mak
