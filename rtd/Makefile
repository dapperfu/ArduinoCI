# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile
CWD = $(realpath $(dir $(firstword $(MAKEFILE_LIST))))



MONITOR_BAUDRATE ?= 115200

# Board Configuration
VENDOR ?= arduino
ARCHITECTURE ?= avr


ARDUINO_VERSION = 185
BOARD_TAG   = uno
BOARD_SUB   = atmega328p


# Configure directory paths.
WORKSPACE ?= $(realpath $(dir $(firstword $(MAKEFILE_LIST)))..)
ARDUINO_DIR ?= ${WORKSPACE}/arduino
ARDMK_DIR ?= ${WORKSPACE}/arduino_make

USER_LIB_PATH ?= ${CWD}/libraries

# Include the Arduino-Makefile project makefile.
include $(ARDMK_DIR)/Arduino.mk
