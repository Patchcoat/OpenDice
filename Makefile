# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -g -Wall -lm

# the target executable
OBJFILES = roll.o graph.o blumblumshub.o
TARGET = roll

# build the program
all: $(TARGET)

$(TARGET): $(OBJFILES)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJFILES)

$(TARGET).o: $(TARGET).c graph.h blumblumshub.h
	$(CC) $(CFLAGS) -c $(TARGET).c

graph.o: graph.c graph.h
	$(CC) $(CFLAGS) -c graph.c

blumblumshub.o: blumblumshub.c blumblumshub.h
	$(CC) $(CFLAGS) -c blumblumshub.c

clean:
	$(RM) $(OBJFILES) $(TARGET)
