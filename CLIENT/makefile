# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I/usr/include -I/usr/local/include

# Linker settings
LDFLAGS = -L/usr/lib -L/usr/local/lib -lboost_system -pthread

# Executable name
TARGET = main

# Source files
SRC = main.cpp
OBJ = $(SRC:.cpp=.o)

# Default target
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $^ -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
