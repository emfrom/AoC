CC = gcc
CFLAGS = -Wall -Wextra -g -pg

TARGET = day25
SRC = $(TARGET).c
OBJ = $(TARGET).o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
