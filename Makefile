CXX = g++
CXXFLAGS = -I/home/ubuntu/Desktop/googletest/googletest/include -std=c++17
LDFLAGS = -I/home/ubuntu/Desktop/googletest/build -lgtest -lgtest_main -lpthread

SRC = unit_tests.cpp
OBJ = $(SRC:.cpp=.o)
EXE = unit_tests


all: $(EXE)

$(EXE): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXE)

.PHONY: clean