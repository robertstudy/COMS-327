CXX = g++  

CXXFLAGS = -std=c++11 -Wall

SRCS = connect_four.cpp

TARGET = connect_four

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lncurses

.PHONY: clean

clean:
	rm -f $(TARGET) 