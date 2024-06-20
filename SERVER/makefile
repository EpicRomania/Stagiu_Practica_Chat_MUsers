# Compiler settings - Can be customized.
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2
LDFLAGS = -lSQLiteCpp -lsqlite3 -lpthread -ldl

# Define your source files here.
SOURCES = main.cpp SQLiteComponent.cpp server.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean
