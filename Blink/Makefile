# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile
WORKSPACE ?= $(realpath $(dir $(firstword $(MAKEFILE_LIST)))..)

MONITOR_BAUDRATE ?= 115200

BOARD_TAG ?= nano
BOARD_SUB ?= atmega328

ARDUINO_DIR ?= ${WORKSPACE}/arduino

ARDMK_DIR ?= ${WORKSPACE}/arduino_make

include $(ARDMK_DIR)/Arduino.mk
