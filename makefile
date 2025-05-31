LFLAGS = -lX11
CFLAGS = -std=c2x -Wall -Wextra -Wpedantic -g -O0

.PHONY: all
all: snake

%: %.c main.c sys.c
	gcc $(CFLAGS) $^ $(LFLAGS) -o $@
