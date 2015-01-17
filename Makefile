CXXFLAGS = -std=c++11 -Ofast -Wall -Werror
LDFLAGS = -lSDL2
OS = $(shell uname -s)
SRC = $(wildcard *.cpp)
OBJECTS = $(patsubst %.cpp, %.o, $(SRC))
ELFNAME = snake

ifeq ($(OS), Darwin)
	CXX = g++-4.9
endif
ifeq ($(OS), Linux)
	CXX = g++
endif

$(ELFNAME): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o$@ $^

clean:
	rm -f *.o $(ELFNAME)
