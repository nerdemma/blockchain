CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
LDLIBS = -lcrypto -lpthread


SRCS = $(wildcard src/*.c)
OBJS = $(SRCS:.c=.o)
TARGET = blockchain

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all clean