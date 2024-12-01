#!/bin/bash

if [ -z "$1" ]; then
  echo "Usage: $0 <arg>"
  exit 1
fi

arg=$1
dir="$arg"

if [ -d $dir ] ; then
    echo "Directory "$dir" already exists"
    echo "Exiting"
    exit 1
fi

# Create directory
mkdir -p "$dir"

# Create Makefile
cat <<EOF > "$dir/Makefile"
CC = gcc
CFLAGS = -Wall -Wextra -g -pg

TARGET = day$arg
SRC = \$(TARGET).c
OBJ = \$(TARGET).o

all: \$(TARGET)

\$(TARGET): \$(OBJ)
	\$(CC) -o \$(TARGET) \$(OBJ)

\$(OBJ): \$(SRC)
	\$(CC) \$(CFLAGS) -c \$(SRC)

clean:
	rm -f \$(OBJ) \$(TARGET)

.PHONY: all clean
EOF

# Create C file
cat <<EOF > "$dir/day$arg.c"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main() {


  return EXIT_SUCCESS;
}
EOF
echo "Project setup complete in $dir"
