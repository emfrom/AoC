#!/bin/bash

if [ -z "$1" ]; then
  echo "Usage: $0 <arg>"
  exit 1
fi

arg=$1
dir="$arg"

if [ -d $dir ] ; then
    echo "Directory "$dir" already exists"
else
    # Create directory
    mkdir -p "$dir"
fi

if [ -f $dir/Makefile ] ; then
    echo "Makefile already exists"
else
    # Create Makefile
    cat <<EOF > "$dir/Makefile"
CC = gcc
CFLAGS = -Wall -Wextra -g -pg -DDEBUG
CFLAGS += -I/usr/local/include
CFLAGS += -I/home/emfrom/include
LDFLAGS = -L/usr/local/lib
LDFLAGS += -L/home/emfrom/lib 
LIBS = 

TARGET = day$arg
SRC = \$(TARGET).c
OBJ = \$(TARGET).o


all: \$(TARGET)

\$(TARGET): \$(OBJ)
	\$(CC) -o \$(TARGET) \$(OBJ) \$(LDFLAGS) \$(LIBS)

\$(OBJ): \$(SRC)
	\$(CC) \$(CFLAGS) -c \$(SRC) -o \$(OBJ)

clean:
	rm -f \$(OBJ) \$(TARGET)

.PHONY: all clean
EOF
fi

if [ -f $dir/day$arg.c ] ; then
   echo "C file already exists"
else
	# Create C file
	cat <<EOF > "$dir/day$arg.c"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include "../utility_blob.c"


int main() {


  return EXIT_SUCCESS;
}
EOF
fi

#All done
echo "Project setup complete in $dir"
