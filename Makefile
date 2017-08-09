## Configuration
# Versions to get.
ARDUINO_VERSION ?= 1.8.3
ARDUINO_MK_VERSION ?= 1.5

## Setup
# URLS to download.
ARDUINO_MK_URL = https://github.com/sudar/Arduino-Makefile/archive/${ARDUINO_MK_VERSION}.tar.gz
ARDUINO_URL = https://github.com/arduino/Arduino/archive/${ARDUINO_VERSION}.tar.gz

# Download Command
DOWNLOAD_CMD ?= curl --silent --location --output

# Workspace directory minus trailing slash.
# If not set from Jenkins set to directory of Makefile
WORKSPACE ?= $(realpath $(dir $(firstword $(MAKEFILE_LIST))))

## Make Targets
.DEFAULT:
default:
	@echo No default rule

# Do everything.
.PHONY: all
all: env
	@echo Done.

## Environment Setup
.PHONY: env
env: arduino arduino_make
	@echo Setup Environment.

# Clean arduino & arduino_make folders.
.PHONY: clean
clean:
	@echo Deleting directories...
	@rm -rf arduino arduino_make

# Clean everything, including cached downloads.
.PHONY: cleanall
cleanall: clean
	@echo Deleting cached archives...
	@rm -rf *.tar.gz

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

# Blink Project
.PHONY: Blink
Blink:
	@echo Building $@...
	ARDUINO_VERSION=$(subst .,,${ARDUINO_VERSION}) $(MAKE) -j4 -C $@

.PHONY: CAN_Bus_Shield
CAN_Bus_Shield:
	@echo Building $@...
	ARDUINO_VERSION=$(subst .,,${ARDUINO_VERSION}) $(MAKE) -j4 -C $@


## Computer Setup

# OS Detection
ifeq ($(OS),Windows_NT)
    detected_OS := Windows
else
    detected_OS := $(shell uname -s)
endif

# Install the compiler.
.PHONY: compiler
compiler:
ifeq (${detected_OS}, Linux)
	@echo Installing compiler for Linux
	@sudo apt-get install gcc-avr binutils-avr gdb-avr avr-libc avrdude
endif
