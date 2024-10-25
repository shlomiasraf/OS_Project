# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread -g -fprofile-arcs -ftest-coverage  # Compiler flags with debug information and coverage support

# Source files
SRCS = ActiveObject.cpp Graph.cpp main.cpp Pipeline.cpp \
       CommandExecute.cpp kruskalMST.cpp MSTInfo.cpp primMST.cpp

# Header files
HEADERS = ActiveObject.hpp Graph.hpp Pipeline.hpp \
          CommandExecute.hpp kruskalMST.hpp MSTInfo.hpp primMST.hpp

# Output executable
TARGET = pipeline_app

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

# Clean up the build
clean:
	rm -f $(TARGET) *.o *.gcno *.gcda  # Clean coverage files as well

# Phony targets
.PHONY: all clean

