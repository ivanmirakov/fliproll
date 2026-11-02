CC      = clang
CFLAGS  = -std=c17 -Wall -Wextra -g -fsanitize=address,undefined \
          $(shell pkg-config --cflags sdl3)
LDFLAGS = -fsanitize=address,undefined $(shell pkg-config --libs sdl3)

SRC = $(wildcard src/*.c)
OUT = drawing

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)

.PHONY: run clean