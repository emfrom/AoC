CC = gcc
CFLAGS = -Wall -Wextra -g -pg -DDEBUG
CFLAGS += -I/usr/local/include
CFLAGS += -I/home/emfrom/include
LDFLAGS = -L/usr/local/lib
LDFLAGS += -L/home/emfrom/lib 
LIBS = 

TARGET = day3
SRC = $(TARGET).c
OBJ = $(TARGET).o


all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS) $(LIBS)

$(OBJ): $(SRC)
	$(CC) $(CFLAGS) -c $(SRC) -o $(OBJ)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
