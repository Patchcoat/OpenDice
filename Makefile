# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -g -Wall -lm

# the target executable
TARGET = roll

# build the program
all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
