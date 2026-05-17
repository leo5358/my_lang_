CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

TARGET = cpplox

SRC = main.cpp lexer.cpp parser.cpp interpreter.cpp token.cpp
OBJ = $(SRC:.cpp=.o)
HEADERS = ast.h environment.h interpreter.h lexer.h parser.h token.h token_type.h

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
