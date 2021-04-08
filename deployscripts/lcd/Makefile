TOP=${PWD}
# you can add toolchain path here
INC=${TOP}/include
SRC=${TOP}/src
BIN=${TOP}/bin
# we have determined the toolchain prefix, let's determine CC/LD to be used here
CC=${HOST_PREFIX}gcc

.PHONY: all
OBJCPY    := objcopy

SRCFILE1  := $(SRC)/ezio_g500.c  $(SRC)/lcd_ctrl.c
SRCFILE2  := $(SRC)/ezio_300.c  $(SRC)/lcd_ctrl.c
SRCFILE3  := $(SRC)/ezio_340.c  $(SRC)/lcd_ctrl.c
SRCFILE4  := $(SRC)/ezio_390.c  $(SRC)/lcd_ctrl.c

BINFILE1  := $(BIN)/ezio_g500_api
BINFILE2  := $(BIN)/ezio_300_api
BINFILE3  := $(BIN)/ezio_310_api
BINFILE4  := $(BIN)/ezio_320_api
BINFILE5  := $(BIN)/ezio_330_api
BINFILE6  := $(BIN)/ezio_340_api
BINFILE7  := $(BIN)/ezio_350_api
BINFILE8  := $(BIN)/ezio_360_api
BINFILE9  := $(BIN)/ezio_g400_api
BINFILE10 := $(BIN)/ezio_370_api
BINFILE11 := $(BIN)/ezio_380_api
BINFILE12 := $(BIN)/ezio_390_api

OBJPARA   := -R .comment -R .note.ABI-tag -R .gnu.version
CCPARA    := -Os -s -I ./ -L ./

default : all

init_env:
	@ [ -d ${BIN} ]  || mkdir -p ${BIN}

300: init_env
	$(CC) -DEZIO_300_CONFIG $(CCPARA) $(SRCFILE2)  -o $(BINFILE2)
	- ln -s ${BIN}/ezio_300_api ${BIN}/ezio_g300_api
310: init_env
	$(CC) -DEZIO_300_CONFIG $(CCPARA) $(SRCFILE2)  -o $(BINFILE3)
320: init_env
	$(CC) -DEZIO_300_CONFIG -DEZIO_320_CONFIG $(CCPARA) $(SRCFILE2)  -o $(BINFILE4)
330: init_env
	$(CC) -DEZIO_300_CONFIG $(CCPARA) $(SRCFILE2)  -o $(BINFILE5)
340: init_env
	$(CC) -DEZIO_340_CONFIG $(CCPARA) $(SRCFILE3) -o $(BINFILE6)
350: init_env
	$(CC) -DEZIO_300_CONFIG $(CCPARA) $(SRCFILE2)  -o $(BINFILE7)
360: init_env
	$(CC) -DEZIO_300_CONFIG -DENABLE_STOP $(CCPARA) $(SRCFILE2)  -o $(BINFILE8)
370: init_env
	$(CC) -DEZIO_300_CONFIG $(CCPARA) $(SRCFILE2)  -o $(BINFILE10)
380: init_env
	$(CC) -DEZIO_300_CONFIG -DEZIO_380_CONFIG $(CCPARA) $(SRCFILE2)  -o $(BINFILE11)
390: init_env
	$(CC) -DEZIO_390_CONFIG $(CCPARA) $(SRCFILE4) -o $(BINFILE12)
g400: init_env
	$(CC) -DEZIO_G500_CONFIG -DEZIO_G400_CONFIG $(CCPARA) $(SRCFILE1)  -o $(BINFILE9)
g500: init_env
	$(CC) -DEZIO_G500_CONFIG $(CCPARA) $(SRCFILE1)  -o $(BINFILE1)

all: 300 310 320 330 340 350 360 370 380 390 g400 g500

clean:
	rm -rf ${BIN}
