CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS = -lrt -pthread

TARGET = ipc_analyzer

SRC = src/main.cpp \
      src/benchmark.cpp \
      src/configuration.cpp \
      src/logger.cpp \
      src/pipe_test.cpp \
      src/message_queue_test.cpp \
      src/shared_memory_test.cpp

OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
