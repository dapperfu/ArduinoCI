## Configuration
# Versions to get.
ARDUINO_VERSION ?= 1.8.3
ARDUINO_MK_VERSION ?= 1.5

## Setup
# URLS to download.
ARDUINO_MK_URL = https://github.com/sudar/Arduino-Makefile/archive/${ARDUINO_MK_VERSION}.tar.gz
ARDUINO_URL = https://github.com/arduino/Arduino/archive/${ARDUINO_VERSION}.tar.gz

# Archive Files
ARCHIVES = arduino.tar.gz arduino_make.tar.gz

# Directories
DIRS = $(patsubst %.tar.gz, %, $(ARCHIVES))

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
env: ${DIRS}
	@echo Setup Environment.

# Clean archives and folders.
.PHONY: clean
clean:
	@echo Deleting directories...
	@rm -rf ${DIRS}
	@echo Deleting archives...
	@rm -rf *.tar.gz

# Download the Arduino release.
arduino.tar.gz:
	@echo Downloading $@...
	@${DOWNLOAD_CMD} $@ ${ARDUINO_URL}

# Download the Arduino Make release.
arduino_make.tar.gz:
	@echo Downloading $@...
	@${DOWNLOAD_CMD} $@ ${ARDUINO_MK_URL}

# Make directories and expand archives.
$(DIRS): $(ARCHIVES)
	@echo Making $@ directory...
	@mkdir -p $@
	@echo Extracting $@.tar.gz to $@...
	@tar -xzf $@.tar.gz -C $@ --strip=1

## Project Builds

# Blink Project
.PHONY: Blink
Blink:
	@echo Building $@...
	ARDUINO_VERSION=$(subst .,,${ARDUINO_VERSION}) $(MAKE) -C $@

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
