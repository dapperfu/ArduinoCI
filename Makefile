## Configuration
# Versions to get.
ARDUINO_VERSION ?= 1.8.10
ARDUINO_MK_VERSION ?= 1.6.0

## Setup
# URLS to download.
ARDUINO_MK_URL = https://github.com/sudar/Arduino-Makefile/archive/${ARDUINO_MK_VERSION}.tar.gz
ARDUINO_URL = https://github.com/arduino/Arduino/archive/${ARDUINO_VERSION}.tar.gz
SANGUINO_URL = https://github.com/Lauszus/sanguino/tarball/master
U8GLIB_URL = https://bintray.com/olikraus/u8glib/download_file?file_path=u8glib_arduino_v1.18.1.zip

# Download Command
DOWNLOAD_CMD ?= curl --silent --location --output

# Workspace directory minus trailing slash.
# If not set from Jenkins set to directory of Makefile
WORKSPACE ?= $(realpath $(dir $(firstword $(MAKEFILE_LIST))))

# Files to determine if a stage has been completed
U8GLIB = arduino/libraries/U8glib/INSTALL.TXT
SANGUINO = arduino/hardware/arduino/avr/variants/sanguino/pins_arduino.h

# Temporary directory to extract sanguino to
SANGUINO_TMP = /tmp/sanguino

## Make Targets
## Environment Setup
.DEFAULT: env
.PHONY: env
env: arduino arduino_make ${U8GLIB} ${SANGUINO}
	@echo Environment Setup.

${U8GLIB}: u8glib_arduino.zip arduino
# Extract the u8glib library.
	@unzip -o ${<} -d arduino/libraries/

${SANGUINO}: sanguino.tar.gz arduino
# Make temp directory to extract into
	@mkdir -p ${SANGUINO_TMP}
# Extract, strip the top level.
	@tar --strip-components=1 -C ${SANGUINO_TMP} -xzvf ${<}
# Copy over required files.
	@mv -n ${SANGUINO_TMP}/avr/bootloaders/optiboot/* arduino/hardware/arduino/avr/bootloaders/optiboot/
	@mv -n ${SANGUINO_TMP}/avr/variants/sanguino/ arduino/hardware/arduino/avr/variants/
# Strip out the 'cpu.menu' line and echo into the available avr boards file.
	@grep -v "cpu.menu" ${SANGUINO_TMP}/avr/boards.txt >> arduino/hardware/arduino/avr/boards.txt

sanguino.tar.gz:
	@${DOWNLOAD_CMD} ${@} ${SANGUINO_URL}

u8glib_arduino.zip:
	@${DOWNLOAD_CMD} ${@} ${U8GLIB_URL}

# Clean arduino & arduino_make folders.
.PHONY: clean
clean:
	@echo Cleaning directories...
	@git clean -xfd

# Download the Arduino release.
arduino_${ARDUINO_VERSION}.tar.gz:
	@echo Downloading $@...
	@${DOWNLOAD_CMD} $@ ${ARDUINO_URL}

# Extract into arduino folder.
arduino: arduino_${ARDUINO_VERSION}.tar.gz
	@mkdir -p $@
	@tar -xzf $< -C $@ --strip=1

# Download the Arduino Make release.
arduino_make_${ARDUINO_MK_VERSION}.tar.gz:
	@echo Downloading $@...
	@${DOWNLOAD_CMD} $@ ${ARDUINO_MK_URL}

arduino_make: arduino_make_${ARDUINO_MK_VERSION}.tar.gz
	@mkdir -p $@
	@tar -xzf $< -C $@ --strip=1

## Project Builds

# Marlin
.PHONY: Blink
Blink: env
	@echo Building $@...
	ARDUINO_VERSION=$(subst .,,${ARDUINO_VERSION}) $(MAKE) -j4 -C $@

.PHONY: CAN_Bus_Shield
CAN_Bus_Shield:
	@echo Building $@...
	ARDUINO_VERSION=$(subst .,,${ARDUINO_VERSION}) $(MAKE) -j4 -C $@
