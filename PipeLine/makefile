# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread -g -fprofile-arcs -ftest-coverage # Compiler flags with debug & coverage info

# Source files
SRCS = ActiveObject.cpp Graph.cpp main.cpp Pipeline.cpp \
       CommandExecute.cpp kruskalMST.cpp MSTInfo.cpp primMST.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Header files
HEADERS = ActiveObject.hpp Graph.hpp Pipeline.hpp \
          CommandExecute.hpp kruskalMST.hpp MSTInfo.hpp primMST.hpp

# Output executable
TARGET = LF_pipeline

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
	# Generate coverage reports, excluding unwanted directories and files
	gcov -b -r -o . $(SRCS) | grep -v '/usr/include/' | grep -v 'bits/' | grep -v 'ext/' > filtered_coverage.txt

	# Display filtered coverage report
	cat filtered_coverage.txt

# Clean up the build
clean:
	rm -f $(TARGET) $(OBJS) *.gcno *.gcda *.gcov filtered_coverage.txt

# Phony targets
.PHONY: all clean coverage
