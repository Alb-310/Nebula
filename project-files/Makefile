# change application name here (executable output name)
TARGET=nebula

# compiler
CC=gcc
# debug
DEBUG=-g
# optimisation
OPT=-O0
# warnings
WARN=-Wall -Wextra

PTHREAD=-pthread

CCFLAGS=$(DEBUG) $(OPT) $(WARN) $(PTHREAD) -pipe

GTKLIB=`pkg-config --cflags --libs gtk+-3.0`

# linker
LD=gcc
LDFLAGS=$(PTHREAD) $(GTKLIB) $(SDLLIB) -export-dynamic -rdynamic -lm

OBJS=    src/.o/main.o

all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)
src/.o/main.o: src/main.c
	$(CC) -c $(CCFLAGS) src/main.c $(GTKLIB) $(SDLLIB) -o src/.o/main.o    
clean:
	rm -f src/.o/*.o $(TARGET) -r cache/*
