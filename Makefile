CC = cc
CFLAGS = -std=c99 -Wall -Wextra $(shell pkg-config --cflags sdl3)
LDLIBS = $(shell pkg-config --libs sdl3)

SRCS = src/main.c src/particle.c
OBJS = $(SRCS:.c=.o)
TARGET = hyperspace

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDLIBS) -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
