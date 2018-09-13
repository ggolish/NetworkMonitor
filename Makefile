CFLAGS = -Wall -g -Iinclude
LIBS = -lncurses
CC = gcc

.SUFFIXES: .c .o

.c.o:
	$(CC) -c $(CFLAGS) $*.c -o $*.o

OBJS = \
	src/errors.c	\
	src/main.c	\
	src/netmon.c	\
	src/ui.c	\

TARGET = netmon

default: $(TARGET)

$(TARGET): $(OBJS:.c=.o)
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LIBS)

errors.o: src/errors.c include/errors.h

main.o: src/main.c include/netmon.h include/errors.h

netmon.o: src/netmon.c include/netmon.h include/errors.h include/ui.h include/packet.h

ui.o: src/ui.c include/ui.h

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f src/errors.o
	rm -f src/main.o
	rm -f src/netmon.o
	rm -f src/ui.o
	rm -f $(TARGET)
