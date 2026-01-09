# Makefile for CHxPSO-ABS

# Compiler settings
CXX = g++
CXXFLAGS = -std=c++11 -Wall -O3
INCLUDE = -I./include

# Directories
SRC_DIR = src
BUILD_DIR = build
CEC2013_DIR = cec2013
CEC2017_DIR = cec2017

# Target executable
TARGET = chxpso

# Source files
MAIN_SRC = $(SRC_DIR)/main.cpp

# Object files directory
$(shell mkdir -p $(BUILD_DIR))

# Default target - compile basic version without CEC
all: basic

# Basic version (no CEC functions)
basic: $(MAIN_SRC)
	@echo "Compiling basic version (no CEC functions)..."
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(TARGET) $(MAIN_SRC)
	@echo "Build complete! Run with: ./$(TARGET)"

# CEC 2013 version
cec2013: $(MAIN_SRC) $(CEC2013_DIR)/test_func.cpp
	@echo "Compiling with CEC 2013 functions..."
	$(CXX) $(CXXFLAGS) $(INCLUDE) -I$(CEC2013_DIR) -DUSE_CEC2013 \
		-o $(TARGET) $(MAIN_SRC) $(CEC2013_DIR)/test_func.cpp
	@echo "Build complete! Run with: ./$(TARGET)"

# CEC 2017 version (handles .c files)
cec2017: $(MAIN_SRC)
	@echo "Compiling with CEC 2017 functions..."
	@if [ -f "$(CEC2017_DIR)/cec17.c" ]; then \
		$(CXX) $(CXXFLAGS) $(INCLUDE) -I$(CEC2017_DIR) -DUSE_CEC2017 \
			-o $(TARGET) $(MAIN_SRC) \
			$(CEC2017_DIR)/cec17_test_func.c $(CEC2017_DIR)/cec17.c; \
	else \
		$(CXX) $(CXXFLAGS) $(INCLUDE) -I$(CEC2017_DIR) -DUSE_CEC2017 \
			-o $(TARGET) $(MAIN_SRC) $(CEC2017_DIR)/cec17_test_func.c; \
	fi
	@echo "Build complete! Run with: ./$(TARGET)"

# Both CEC 2013 and CEC 2017
cec_all: $(MAIN_SRC) $(CEC2013_DIR)/test_func.cpp $(CEC2017_DIR)/cec17_func.cpp
	@echo "Compiling with CEC 2013 and CEC 2017 functions..."
	$(CXX) $(CXXFLAGS) $(INCLUDE) -I$(CEC2013_DIR) -I$(CEC2017_DIR) \
		-DUSE_CEC2013 -DUSE_CEC2017 \
		-o $(TARGET) $(MAIN_SRC) \
		$(CEC2013_DIR)/test_func.cpp $(CEC2017_DIR)/cec17_func.cpp
	@echo "Build complete! Run with: ./$(TARGET)"

# Debug version (with debug symbols and no optimization)
debug: CXXFLAGS = -std=c++11 -Wall -g
debug: $(MAIN_SRC)
	@echo "Compiling debug version..."
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $(TARGET)_debug $(MAIN_SRC)
	@echo "Debug build complete! Run with: ./$(TARGET)_debug"

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(TARGET) $(TARGET)_debug
	rm -f results.txt
	rm -rf $(BUILD_DIR)
	@echo "Clean complete!"

# Run the program
run: all
	@echo "Running CHxPSO-ABS..."
	./$(TARGET)

# Help target
help:
	@echo "CHxPSO-ABS Makefile"
	@echo "==================="
	@echo ""
	@echo "Available targets:"
	@echo "  make              - Build basic version (default)"
	@echo "  make basic        - Build basic version (no CEC functions)"
	@echo "  make cec2013      - Build with CEC 2013 functions"
	@echo "  make cec2017      - Build with CEC 2017 functions"
	@echo "  make cec_all      - Build with both CEC 2013 and 2017"
	@echo "  make debug        - Build debug version"
	@echo "  make run          - Build and run the program"
	@echo "  make clean        - Remove build artifacts"
	@echo "  make help         - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make              # Compile basic version"
	@echo "  make run          # Compile and run"
	@echo "  make cec2013      # Compile with CEC 2013"
	@echo "  make clean        # Clean everything"

.PHONY: all basic cec2013 cec2017 cec_all debug clean run help