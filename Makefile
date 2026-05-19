# Cardputer LG Remote - PlatformIO wrapper

PIO         ?= pio
ENV         ?= cardputer
UPLOAD_PORT ?=

PIO_FLAGS = -e $(ENV)
ifneq ($(UPLOAD_PORT),)
PIO_FLAGS += --upload-port $(UPLOAD_PORT)
endif

.DEFAULT_GOAL := build
.PHONY: help build flash upload monitor dev clean fullclean size erase \
        rebuild profile-list debug-ir tree

help:
	@echo "Cardputer LG Remote - targets"
	@echo "  make build         Compile firmware"
	@echo "  make flash         Compile + upload to connected device"
	@echo "  make upload        Alias for flash"
	@echo "  make monitor       Serial monitor (115200)"
	@echo "  make dev           Flash then immediately open monitor"
	@echo "  make size          Show firmware RAM/Flash usage"
	@echo "  make erase         Erase entire flash on the device"
	@echo "  make clean         Remove build artifacts (.pio/build/)"
	@echo "  make fullclean     Wipe .pio/ entirely (libdeps + build)"
	@echo "  make rebuild       clean + build"
	@echo "  make tree          Print source tree"
	@echo ""
	@echo "Variables:"
	@echo "  UPLOAD_PORT=/dev/cu.usbmodem101   override autodetect"

build:
	$(PIO) run $(PIO_FLAGS)

flash upload:
	$(PIO) run $(PIO_FLAGS) -t upload

monitor:
	$(PIO) device monitor -e $(ENV)

dev: flash
	$(PIO) device monitor -e $(ENV)

size:
	$(PIO) run $(PIO_FLAGS) -t size

erase:
	$(PIO) run $(PIO_FLAGS) -t erase

clean:
	$(PIO) run $(PIO_FLAGS) -t clean

fullclean:
	rm -rf .pio

rebuild: clean build

tree:
	@find src include -type f \( -name '*.h' -o -name '*.cpp' \) | sort
