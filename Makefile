CFLAGS = -Wall -g -Iinclude
CC = gcc
CLIBS = -lncurses

.SUFFIXES: .c .o

.c.o:
	$(CC) -c $(CFLAGS) $*.c -o $*.o

OBJS = \
	src/errors.c	\
	src/main.c	\
	src/netmon.c	\
	src/rate.c	\
	src/ui.c	\
	src/args.c

TARGET = netmon

default: $(TARGET)

$(TARGET): $(OBJS:.c=.o)
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(CLIBS)

args.o: src/args.c include/args.h include/packet.h include/errors.h

errors.o: src/errors.c include/errors.h

main.o: src/main.c include/netmon.h include/errors.h include/args.h

netmon.o: src/netmon.c include/netmon.h include/errors.h include/ui.h include/packet.h include/rate.h

rate.o: src/rate.c include/rate.h

ui.o: src/ui.c include/ui.h

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f src/errors.o
	rm -f src/main.o
	rm -f src/netmon.o
	rm -f src/rate.o
	rm -f src/ui.o
	rm -f src/args.o
	rm -f $(TARGET)
