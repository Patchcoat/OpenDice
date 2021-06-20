# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -g -Wall -lm

# the target executable
OBJFILES = roll.o graph.o
TARGET = roll

# build the program
all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES)

$(TARGET).o: $(TARGET).c graph.h
	$(CC) $(CFLAGS) -c $(TARGET).c

graph.o: graph.c graph.h
	$(CC) $(CFLAGS) -c graph.c

clean:
	$(RM) $(OBJFILES) $(TARGET)
