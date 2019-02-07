CXX=g++
CXXFLAGS=-Wall -Wextra -Werror -pedantic -std=c++14

LDFLAGS=-lsfml-graphics -lsfml-window -lsfml-system

SRC=src/main.cc

all: $(SRC:.cc=.o)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $^ -o app
	./app --width 600

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f app src/*.o
