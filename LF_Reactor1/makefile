# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall -std=c++17 -fprofile-arcs -ftest-coverage

# Source files
SRCS = Graph.cpp primMST.cpp kruskalMST.cpp ThreadPool.cpp RequestHandling.cpp reactor.cpp main.cpp

# Header files
HEADERS = Graph.hpp primMST.hpp kruskalMST.hpp ThreadPool.hpp RequestHandling.hpp reactor.hpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Output file
OUTPUT = MSTApp

# Default target
all: $(OUTPUT)

# Link the object files to create the executable
$(OUTPUT): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) --coverage

# Compile source files into object files
%.o: %.cpp $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Run tests and generate coverage report
test: $(OUTPUT)
	./$(OUTPUT)
	gcov $(SRCS)

# Clean up build files and coverage files
clean:
	rm -f $(OBJS) $(OUTPUT) *.gcda *.gcno *.gcov

