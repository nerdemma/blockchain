CC = cc
CGLAGS = -Wall -Wextra -Iinclude -g
SRCS = src/utils src/block.c src/blockchain src/main.c
OBJS = $(SRCS:.c=.o)
TARGET = bin/bitcoin_c

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p bin 
	$(CC) $(CFLAGS) -o $	@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $	@
clean:
	rm -f src/*.o $(TARGET)
.PHONY: all clean	