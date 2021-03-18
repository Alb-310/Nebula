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
GDLIB= -lgd -lpng -lz -ljpeg -lfreetype -lm

# linker
LD=gcc
LDFLAGS=$(PTHREAD) $(GDLIB) $(GTKLIB) -export-dynamic -rdynamic -lm

OBJS=    src/.o/main.o src/.o/filters.o src/.o/image_settings.o

all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)
src/.o/main.o: src/main.c
	$(CC) -c $(CCFLAGS) src/main.c $(GDLIB) $(GTKLIB) -o src/.o/main.o  
src/.o/filters.o: src/filters.c
	$(CC) -c $(CCFLAGS) src/filters.c $(GDLIB) $(GTKLIB) -o src/.o/filters.o      
src/.o/image_settings.o: src/image_settings.c
	$(CC) -c $(CCFLAGS) src/image_settings.c $(GDLIB) $(GTKLIB) -o src/.o/image_settings.o  
clean:
	rm -f src/.o/*.o $(TARGET) -r cache/*
