CFLAGS = -Wall -g -Iinclude
CC = gcc

.SUFFIXES: .c .o

.c.o:
	$(CC) -c $(CFLAGS) $*.c -o $*.o

OBJS = \
	src/errors.c	\
	src/main.c	\
	src/netmon.c	\

TARGET = netmon

default: $(TARGET)

$(TARGET): $(OBJS:.c=.o)
	$(CC) $(CFLAGS) $^ -o $(TARGET)

errors.o: src/errors.c include/errors.h

main.o: src/main.c include/netmon.h include/errors.h

netmon.o: src/netmon.c include/netmon.h include/errors.h

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f src/errors.o
	rm -f src/main.o
	rm -f src/netmon.o
	rm -f $(TARGET)
