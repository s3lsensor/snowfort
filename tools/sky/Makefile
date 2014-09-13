ifndef HOST_OS
  ifeq ($(OS),Windows_NT)
    HOST_OS := Windows
  else
    ifneq (,$(findstring raspberrypi,$(shell uname -a)))
      HOST_OS := Pi
    else
      HOST_OS := $(shell uname)
    endif
  endif
endif

FLAG=

ifeq ($(HOST_OS),Windows)
  SERIALDUMP = serialdump-windows
endif

ifeq ($(HOST_OS),Darwin)
  SERIALDUMP = serialdump-macos
endif

ifeq ($(HOST_OS),Pi)
  SERIALDUMP = serialdump-linux
  FLAG+=-DPI
endif

ifndef SERIALDUMP
  # Assume Linux
  SERIALDUMP = serialdump-linux
endif

all:	$(SERIALDUMP)

$(SERIALDUMP):	serialdump.c
	$(CC) $(FLAG) -o $@ $<
