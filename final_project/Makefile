CXX := g++
CXXFLAGS := -O3 -std=c++20

SOURCES := main.cpp Plan.cpp Partition.cpp Module.cpp SA.cpp Contour.cpp
HEADERS := main.h Plan.h Partition.h Module.h SA.h Contour.h

OBJDIR := objs
OBJECTS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(SOURCES))

OUTPUTDIR := floorplan

TARGET := serial_PPF

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJDIR)/%.o: %.cpp $(HEADERS)
	@mkdir -p $(OBJDIR)
	@mkdir -p $(OUTPUTDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJDIR)/*.o $(TARGET)
