# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile

MONITOR_BAUDRATE ?= 115200

# Board Configuration
VENDOR ?= arduino
ARCHITECTURE ?= avr

# Configure directory paths.
WORKSPACE ?= $(realpath $(dir $(firstword $(MAKEFILE_LIST)))..)
ARDUINO_DIR ?= ${WORKSPACE}/arduino
ARDMK_DIR ?= ${WORKSPACE}/arduino_make

USER_LIB_PATH ?= $(realpath $(dir $(firstword $(MAKEFILE_LIST)))libraries)

ARDUINO_LIBS ?= CAN_BUS_Shield SPI

libraries:
	@mkdir -p $@

libraries/CAN_BUS_Shield: libraries
	@git clone https://github.com/Seeed-Studio/CAN_BUS_Shield.git $@


libraries/SPI: libraries
	@cp -R ${ARDUINO_DIR}/hardware/arduino/avr/libraries/SPI libraries/


.DEFAULT:
LIBS: libraries/SPI libraries/CAN_BUS_Shield

# Include the Arduino-Makefile project makefile.
include $(ARDMK_DIR)/Arduino.mk
