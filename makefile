CC = g++
CFLAGS = -Wall -g

# List of source files
CLIENT_SOURCES = client.cpp Graph.cpp
SERVER_SOURCES = server.cpp Graph.cpp primMST.cpp kruskalMST.cpp MSTFactory.cpp

# Output executable names
CLIENT_EXEC = client
SERVER_EXEC = server

all: $(CLIENT_EXEC) $(SERVER_EXEC)

$(CLIENT_EXEC): $(CLIENT_SOURCES)
	$(CC) $(CFLAGS) -o $(CLIENT_EXEC) $(CLIENT_SOURCES)

$(SERVER_EXEC): $(SERVER_SOURCES)
	$(CC) $(CFLAGS) -o $(SERVER_EXEC) $(SERVER_SOURCES)

clean:
	rm -f $(CLIENT_EXEC) $(SERVER_EXEC) *.o
