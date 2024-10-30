# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread -g  # Compiler flags with debug 

# Source files
SRCS = ThreadPool.cpp Graph.cpp main.cpp LFCompute.cpp \
       CommandExecute.cpp kruskalMST.cpp MSTInfo.cpp primMST.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Header files
HEADERS = ThreadPool.hpp Graph.hpp LFCompute.hpp \
          CommandExecute.hpp kruskalMST.hpp MSTInfo.hpp primMST.hpp MSTAlgo.hpp MSTFactory.hpp

# Output executable
TARGET = LF_app

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

