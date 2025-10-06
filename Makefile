CXX = g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra -fconcepts -fcoroutines -I.
TARGET = main
SRCDIR = src
STRATEGIESDIR = $(SRCDIR)/strategies

# Source files
SOURCES = main.cpp \
          $(SRCDIR)/data_loader.cpp \
          $(SRCDIR)/backtester.cpp \
          $(STRATEGIESDIR)/sma_strategy.cpp \
          $(STRATEGIESDIR)/ema_strategy.cpp \
          $(STRATEGIESDIR)/mean_reversion_strategy.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Build the main executable
$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET)

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CXXFLAGS = -std=c++20 -g -Wall -Wextra -fconcepts -fcoroutines -I. -DDEBUG
debug: $(TARGET)

.PHONY: all clean run debug
