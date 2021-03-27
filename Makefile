CXX = nvcc
CXXFLAGS = -c -std=c++17 -O3
LDFLAGS = -O3 -lX11
EXE = fleeca

all: $(EXE)

$(EXE): main.o
	$(CXX) $< $(LDFLAGS) -o $@

main.o: main.cu
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm *.o && rm $(EXE)
