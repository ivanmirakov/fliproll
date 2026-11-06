CC      = clang
CFLAGS  = -std=c17 -Wall -Wextra -g $(shell pkg-config --cflags sdl3)
LDFLAGS = $(shell pkg-config --libs sdl3)

SRC = $(wildcard src/*.c)
OUT = fliproll

ifeq ($(OS),Windows_NT)
    OUT := $(OUT).exe
    CFLAGS  += -gcodeview
    LDFLAGS += -Wl,--pdb=
else
    CFLAGS  += -fsanitize=address,undefined
    LDFLAGS += -fsanitize=address,undefined
endif

$(OUT): $(SRC) Makefile
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT) fliproll.pdb

.PHONY: run clean