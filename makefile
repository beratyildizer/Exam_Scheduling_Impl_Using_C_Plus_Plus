##############
# Makefile for project
##############

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2

# Sources and objects
SOURCES = termProject.cpp examScheduling.cpp
HEADERS = examScheduling.h
OBJECTS = $(SOURCES:.cpp=.o)

# Application name
APP_NAME = app

# Build the application
$(APP_NAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(APP_NAME) $(OBJECTS)

# Compile object files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(OBJECTS) $(APP_NAME)

# Phony targets
.PHONY: clean