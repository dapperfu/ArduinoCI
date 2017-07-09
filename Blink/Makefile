# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile

MONITOR_BAUDRATE ?= 115200

# Board Configuration
VENDOR ?= arduino
ARCHITECTURE ?= avr

# Configure directory paths.
WORKSPACE ?= $(realpath $(dir $(firstword $(MAKEFILE_LIST)))..)
ARDUINO_DIR ?= ${WORKSPACE}/arduino
ARDMK_DIR ?= ${WORKSPACE}/arduino_make

# Include the Arduino-Makefile project makefile.
include $(ARDMK_DIR)/Arduino.mk
