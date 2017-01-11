
# Warn all - always use this to make the compiler really picky (and thus more helpful) 
CFLAGS += -Wall

#include debugging symbols - always use this or your debugger doesn't work! 
CFLAGS += -g

CFLAGS += -I.
CFLAGS += -Ipkg-cron
LINKER_FLAGS += `pkg-config --libs gtk+-3.0` -rdynamic -lm

CFLAGS += `pkg-config --cflags gtk+-3.0`
CFLAGS += -DDEBUGGING=0
CFLAGS += -DDEBIAN=1

#HEADERS = cron_gui.h readCrontab.h
# files to work with:
SOURCES_CRON = pkg-cron/entry.c pkg-cron/env.c pkg-cron/misc.c
SOURCES_BASE = readCrontab.c wizard.c cron_gui.c config_gui_base.c
SOURCES_PROD = main.c
SOURCES_TEST = test.c
TARGET =	cron-gui

# replaces *.c with *.o in the variables 
OBJS_PROD = $(SOURCES_CRON:%.c=%.o) $(SOURCES_BASE:%.c=%.o) $(SOURCES_PROD:%.c=%.o)
OBJS_TEST = $(SOURCES_CRON:%.c=%.o) $(SOURCES_BASE:%.c=%.o) $(SOURCES_TEST:%.c=%.o)

# Link command to compile + build binary:
link:	$(OBJS_PROD)
	gcc -I. $(CFLAGS) $(OBJS_PROD) $(LINKER_FLAGS) -o $(TARGET) $(LIBS)

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
	gcc $(OBJS_TEST) $(LINKER_FLAGS) -o test $(LIBS)
