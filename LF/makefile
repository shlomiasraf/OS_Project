# Compiler
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread -g  # Compiler flags with debug information

# Source files
SRCS = ThreadPool.cpp Graph.cpp main.cpp LFCompute.cpp \
       CommandExecute.cpp kruskalMST.cpp MSTInfo.cpp primMST.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Header files
HEADERS = ThreadPool.hpp Graph.hpp  LFCompute.hpp \
          CommandExecute.hpp kruskalMST.hpp MSTInfo.hpp primMST.hpp

# Output executable
TARGET = LF_app

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

# Compile source files into object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up the build
clean:
	rm -f $(TARGET) *.o

# Phony targets
.PHONY: all clean
