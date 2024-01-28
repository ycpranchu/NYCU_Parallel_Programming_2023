CXX := g++
CXXFLAGS := -O3 -std=c++17 -msse -msse2 -msse3 -msse4 -msse4.1 -msse4.2 -mavx -mavx2

all: pi.cpp
	$(CXX) $(CXXFLAGS) pi.cpp -lpthread -o pi.out

clean:
	rm -f *.out