CXX := mpicxx
CXXFLAGS := -O3 -march=native -std=c++11

SOURCES := main.cc function.cc

OBJECTS := $(SOURCES:.cc=.o)

TARGET := matmul

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)
