# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread -g -fprofile-arcs -ftest-coverage # Compiler flags with debug & coverage info

# Source files
SRCS = ThreadPool.cpp Graph.cpp main.cpp LFCompute.cpp \
       CommandExecute.cpp kruskalMST.cpp MSTInfo.cpp primMST.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Header files
HEADERS = ThreadPool.hpp Graph.hpp LFCompute.hpp \
          CommandExecute.hpp kruskalMST.hpp MSTInfo.hpp primMST.hpp

# Output executable
TARGET = LF_app

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generate code coverage report using gcov
coverage: $(TARGET)
	./$(TARGET)
	gcov -b -o . ThreadPool.cpp Graph.cpp LFCompute.cpp CommandExecute.cpp kruskalMST.cpp MSTInfo.cpp primMST.cpp | \
	grep -v '/usr/include/' | \
	grep -v 'bits/' | \
	grep -v 'ext/'

# Clean up the build
clean:
	rm -f $(TARGET) $(OBJS) *.gcno *.gcda *.gcov

# Phony targets
.PHONY: all clean coverage
