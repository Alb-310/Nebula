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

OBJS=    src/.o/main.o src/.o/filters.o src/.o/image_settings.o src/.o/drawing.o src/.o/func_divers.o src/.o/queue.o src/.o/shared_queue.o


all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)
src/.o/main.o: src/main.c
	$(CC) -c $(CCFLAGS) src/main.c $(GDLIB) $(GTKLIB) -o src/.o/main.o  
src/.o/filters.o: src/filters.c
	$(CC) -c $(CCFLAGS) src/filters.c $(GDLIB) $(GTKLIB) -o src/.o/filters.o      
src/.o/image_settings.o: src/image_settings.c
	$(CC) -c $(CCFLAGS) src/image_settings.c $(GDLIB) $(GTKLIB) -o src/.o/image_settings.o  
src/.o/drawing.o: src/drawing.c
	$(CC) -c $(CCFLAGS) src/drawing.c $(GDLIB) $(GTKLIB) -o src/.o/drawing.o
src/.o/func_divers.o: src/func_divers.c
	$(CC) -c $(CCFLAGS) src/func_divers.c $(GDLIB) $(GTKLIB) -o src/.o/func_divers.o
src/.o/queue.o: src/queue.c
	$(CC) -c $(CCFLAGS) src/queue.c $(GDLIB) $(GTKLIB) -o src/.o/queue.o
src/.o/shared_queue.o: src/shared_queue.c
	$(CC) -c $(CCFLAGS) src/shared_queue.c $(GDLIB) $(GTKLIB) -o src/.o/shared_queue.o
clean:
	rm -f src/.o/*.o $(TARGET) -r cache/*
