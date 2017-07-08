ARDUINO_VER ?= 1.8.3
ARDUINO_MK_VER ?= 1.5

ARDUINO_MK_URL=https://github.com/sudar/Arduino-Makefile/archive/${ARDUINO_MK_VER}.tar.gz
ARDUINO_URL=https://github.com/arduino/Arduino/archive/${ARDUINO_VER}.tar.gz

ARCHIVES = arduino.tar.gz arduino_make.tar.gz
DIRS = $(patsubst %.tar.gz, %, $(ARCHIVES))

arduino.tar.gz:
	curl -L -o $@ ${ARDUINO_URL}

arduino_make.tar.gz:
	curl -L -o $@ ${ARDUINO_MK_URL}

$(DIRS): $(ARCHIVES)
	mkdir -p $@
	tar -xzf $@.tar.gz -C $@ --strip=1

.PHONY: extract
extract: $(DIRS)

.PHONY: all
all: extract

.PHONY: clean
clean:
	rm -rf ${DIRS}
#	rm -rf *.tar.gz

.PHONY: sandbox
sandbox: ${DIRS}

.PHONY: blink
blink:
