CXX = c++
SDL_LIB = -L/usr/local/lib -lSDL2 -Wl,-rpath=/usr/local/lib
SDL_INCLUDE = -I/usr/local/include
CXXFLAGS = -Wall -Wextra -Werror -pedantic -c -std=gnu++17 -Ofast $(SDL_INCLUDE)
LDFLAGS = -Ofast $(SDL_LIB)
EXE = fleeca

all: $(EXE)

$(EXE): main.o
	$(CXX) $< $(LDFLAGS) -o $@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm *.o && rm $(EXE)
