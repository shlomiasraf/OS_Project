# Compiler
CC = g++

# Compiler flags
CFLAGS = -Wall

# Source files
SRCS = ActiveObject.cpp Graph.cpp primMST.cpp kruskalMST.cpp CommandExecuteStage.cpp CommunicationStage.cpp ResultProcessingStage.cpp main.cpp

# Header files
HEADERS = ActiveObject.hpp Graph.hpp primMST.hpp kruskalMST.hpp CommandExecuteStage.hpp CommunicationStage.hpp ResultProcessingStage.hpp MSTFactory.hpp

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
