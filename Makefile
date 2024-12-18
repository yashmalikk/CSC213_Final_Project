# Makefile for the secure communication project

CC = gcc               # Compiler to use
CFLAGS = -Wall -g -w   # Compiler flags, enable all warnings and debugging info
LDFLAGS =              # Linker flags (empty for now)
LIBS =                 # Libraries to link with (empty for now)

# Source files in the root directory
SRC = chat.c \
      message.c \
      socket.c \
      key_management.c \
      utility.c
OBJ = $(SRC:.c=.o)

# Executable name
EXEC = secure_communication

# Default target
all: $(EXEC)

# Rule to create the executable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS) $(LIBS)

# Rule to compile each source file into object file
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to clean up object files and executable
clean:
	rm -f $(OBJ) $(EXEC)

# Rule to run the program
run: $(EXEC)
	./$(EXEC)
