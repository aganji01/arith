# Makefile for arith (Comp 40 Assignment 4)
# 
# Includes build rules for a2test and ppmtrans.
#
# This Makefile is more verbose than necessary.  In each assignment
# we will simplify the Makefile using more powerful syntax and implicit rules.
#
# Last updated: February 16, 2016


############## Variables ###############

CC = gcc # The compiler being used

# Updating include path to use Comp 40 .h files and CII interfaces
IFLAGS = -I/comp/40/build/include -I/usr/sup/cii40/include/cii

# Compile flags
# Set debugging information, allow the c99 standard,
# max out warnings, and use the updated include path
# CFLAGS = -g -std=c99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)
# 
# For this assignment, we have to change things a little.  We need
# to use the GNU 99 standard to get the right items in time.h for the
# the timing support to compile.
# 
CFLAGS = -g -std=gnu99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

# Linking flags
# Set debugging information and update linking path
# to include course binaries and CII implementations
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64

# Libraries needed for linking
# All programs cii40 (Hanson binaries) and *may* need -lm (math)
# 40locality is a catch-all for this assignment, netpbm is needed for pnm
# rt is for the "real time" timing library, which contains the clock support
LDLIBS = -larith40 -l40locality -lnetpbm -lpnmrdr -lcii40 -lm -lpnm

# Collect all .h files in your directory.
# This way, you can never forget to add
# a local .h file in your dependencies.
#
# This bugs Mark, who dislikes false dependencies, but
# he agrees with Noah that you'll probably spend hours 
# debugging if you forget to put .h files in your 
# dependency list.
INCLUDES = $(shell echo *.h)

############### Rules ###############

all: ppmdiff 40image


## Compile step (.c files -> .o files)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@


## Linking step (.o files -> executable programs)

# Build the main executable '40image' with all necessary object files.
40image: 40image.o compress40.o bitpack.o \
         image_processing.o color_conversion.o \
         chroma_processing.o transform.o quantization.o io.o uarray2.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# Build the 'ppmdiff' executable.
# Assuming 'ppmdiff.c' exists and requires only 'ppmdiff.o'.
# If 'ppmdiff' depends on other object files, add them accordingly.
ppmdiff: ppmdiff.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

## Clean rule
clean:
	rm -f 40image ppmdiff *.o