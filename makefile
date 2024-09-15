# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall 

# Source files
SRCS = Graph.cpp primMST.cpp kruskalMST.cpp MSTFactory.cpp proactor.cpp

# Header files
HEADERS = Graph.hpp primMST.hpp kruskalMST.hpp MSTFactory.hpp proactor.hpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Output file
OUTPUT = MSTFactory

# Default target
all: $(OUTPUT)

# Link the object files to create the executable
$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

# Compile source files into object files
%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(OUTPUT)
