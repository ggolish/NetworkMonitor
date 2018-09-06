CFLAGS = -Wall -g -Iinclude
CC = gcc

.SUFFIXES: .c .o

.c.o:
	$(CC) -c $(CFLAGS) $*.c -o $*.o

OBJS = \
	src/main.c	\

TARGET = target

default: $(TARGET)

$(TARGET): $(OBJS:.c=.o)
	$(CC) $(CFLAGS) $^ -o $(TARGET)

main.o: src/main.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f src/main.o
	rm -f $(TARGET)
