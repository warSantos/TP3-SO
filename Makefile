#Main Makefile
CC := gcc
CFLAGS := -g -Wall -MMD

#Directories
IDIR := ./include
SDIR := ./src

ODIR := ./obj

#Paths
INCLUDE_PATHS = $(IDIR:%=-I%)

#Libraries
LIBS := 
#CFLAGS+= `pkg-config --cflags $(LIBS)`
#LIBRARIES = `pkg-config --libs $(LIBS)`

#Compilation line
COMPILE = $(CC) $(CFLAGS) $(INCLUDE_PATHS)

#FILEs
#---------------Source----------------#
SRCS = $(wildcard $(SDIR)/*.c)
#---------------Object----------------#
OBJS = $(SRCS:$(SDIR)/%.c=$(ODIR)/%.o)
#-------------Dependency--------------#
DEPS = $(SRCS:$(SDIR)/%.c=$(ODIR)/%.d)


all: mkobj	$(OBJS)
	$(COMPILE) $(OBJS) main.c -o fat $(LIBRARIES)


mkobj:
	mkdir -p obj

# Include all .d files
-include $(DEPS)

$(ODIR)/%.o: $(SDIR)/%.c
	$(COMPILE) -c $< -o $@ $(LIBRARIES)

.PHONY : clean
clean:
	-rm -rf *.d obj/*
