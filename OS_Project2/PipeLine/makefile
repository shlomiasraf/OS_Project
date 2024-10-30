# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread -g # Compiler flags with debug information

# Source files
SRCS = ActiveObject.cpp Graph.cpp main.cpp Pipeline.cpp \
       CommandExecute.cpp kruskalMST.cpp MSTInfo.cpp primMST.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Header files
HEADERS = ActiveObject.hpp Graph.hpp Pipeline.hpp \
          CommandExecute.hpp kruskalMST.hpp MSTInfo.hpp primMST.hpp MSTAlgo.hpp MSTFactory.hpp

# Output executable
TARGET = pipeline_app

# Default target
all: $(TARGET)

# Compile source files into object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# Clean up the build
clean:
	rm -f $(TARGET) $(OBJS)

# Phony targets
.PHONY: all clean coverage

