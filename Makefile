
# Warn all - always use this to make the compiler really picky (and thus more helpful) 
COMPILER_FLAGS += -Wall

#include debugging symbols - always use this or your debugger doesn't work! 
COMPILER_FLAGS += -g

COMPILER_FLAGS += -I.
COMPILER_FLAGS += -Ipkg-cron
LINKER_FLAGS += `pkg-config --libs gtk+-3.0` -rdynamic

COMPILER_FLAGS += `pkg-config --cflags gtk+-3.0`
COMPILER_FLAGS += -DDEBUGGING=0
COMPILER_FLAGS += -DDEBIAN=1

# files to work with:
SOURCES = 	pkg-cron/entry.c pkg-cron/env.c pkg-cron/misc.c main.c
#OBJS = 	main.o entry.o 
TARGET =	cron-gui

# .o files have same name as .cpp files
OBJS = $(SOURCES:%.c=%.o)

# Link command to compile + build binary:
link:	$(OBJS)
	gcc entry.o env.o misc.o main.o $(LINKER_FLAGS) -o $(TARGET) $(LIBS)

main.o: main.c
	gcc $(COMPILER_FLAGS) -DMAIN_PROGRAM=1 -c $<
	
# Compilation command:
# Generic Way to create a .o file for each .c file (for many c files) $< is the "first dependency"
%.o: %.c
	gcc $(COMPILER_FLAGS) -c $<

#support for "make all"
all: link
	@echo "-- build of target: '$(TARGET)' finished --"

#support for "make clean"
clean:
	rm -f $(OBJS) $(TARGET) $(LIBRARY)
