CC = gcc
CFLAGS = -Wall -Wextra -std=c99
TARGETS = mar unmar

ifeq ($(OS),Windows_NT)
    EXE_EXT = .exe
    RM = del /Q
else
    EXE_EXT =
    RM = rm -f
endif

all: $(TARGETS)

mar: moerchive.c moerchive.h
	$(CC) $(CFLAGS) -o mar$(EXE_EXT) moerchive.c

unmar: unmoerchive.c moerchive.h
	$(CC) $(CFLAGS) -o unmar$(EXE_EXT) unmoerchive.c

clean:
ifeq ($(OS),Windows_NT)
	$(RM) mar.exe unmar.exe
else
	$(RM) mar unmar
endif

test-gcc:
	$(MAKE) CC=gcc

test-clang:
	$(MAKE) CC=clang

info:
	@echo "Compiler: $(CC)"
	@echo "Platform: $(OS)"
	@echo "Targets: $(TARGETS)"

.PHONY: all clean test-gcc test-clang info