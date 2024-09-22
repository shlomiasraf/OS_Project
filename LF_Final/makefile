# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -std=c++17

# Source files
SRCS = Graph.cpp primMST.cpp kruskalMST.cpp ThreadPool.cpp RequestHandling.cpp HandleSet.cpp main.cpp

# Header files
HEADERS = Graph.hpp primMST.hpp kruskalMST.hpp ThreadPool.hpp RequestHandling.hpp HandleSet.hpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Output file
OUTPUT = MSTApp

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
