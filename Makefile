
# Warn all - always use this to make the compiler really picky (and thus more helpful) 
CFLAGS += -Wall

#include debugging symbols - always use this or your debugger doesn't work! 
CFLAGS += -g

CFLAGS += -I.
CFLAGS += -Ipkg-cron
LINKER_FLAGS += `pkg-config --libs gtk+-3.0` -rdynamic

CFLAGS += `pkg-config --cflags gtk+-3.0`
CFLAGS += -DDEBUGGING=0
CFLAGS += -DDEBIAN=1

#HEADERS = cron_gui.h readCrontab.h
# files to work with:
SOURCES = pkg-cron/entry.c pkg-cron/env.c pkg-cron/misc.c readCrontab.c cron_gui.c main.c
SOURCES_TEST = 	pkg-cron/entry.c pkg-cron/env.c pkg-cron/misc.c readCrontab.c cron_gui.c test.c
#OBJS = 	main.o entry.o 
TARGET =	cron-gui

# .o files have same name as .cpp files
OBJS = $(SOURCES:%.c=%.o)
OBJS_TEST = $(SOURCES_TEST:%.c=%.o)

# Link command to compile + build binary:
link:	$(OBJS)
	gcc -I. entry.o env.o misc.o readCrontab.o cron_gui.o main.o $(LINKER_FLAGS) -o $(TARGET) $(LIBS)

main.o: main.c
	gcc $(CFLAGS) -DMAIN_PROGRAM=1 -c $<
	
test.o: test.c
	gcc $(CFLAGS) -DMAIN_PROGRAM=1 -c $<

# Compilation command:
# Generic Way to create a .o file for each .c file (for many c files) $< is the "first dependency"
%.o: %.c
	gcc -c -o $@ $< $(CFLAGS)
	
#support for "make all"
all: link
	@echo "-- build of target: '$(TARGET)' finished --"

#support for "make clean"
clean:
	rm -f $(OBJS) $(OBJS_TEST) $(TARGET) test
	
test: $(OBJS_TEST)
	gcc entry.o env.o misc.o readCrontab.o cron_gui.o test.o $(LINKER_FLAGS) -o test $(LIBS)
