SDL_CFLAGS := $(shell pkg-config --cflags sdl3)
SDL_LIBS   := $(shell pkg-config --libs sdl3)

NATIVE_BIN = main.bin
EMCC = em++
WEB_FLAGS = -std=c++17 -O3 -sALLOW_MEMORY_GROWTH=1 --preload-file Assets
WEB_OUT = docs/index.html

CXX = clang
CXXFLAGS = -std=c++17 -g $(SDL_CFLAGS)

build: clean
	g++ $(CXXFLAGS) main.cpp -o $(NATIVE_BIN) $(SDL_LIBS) -lGL

run_main: build
	./main.bin

build_web: clean
	mkdir -p docs
	$(EMCC) main.cpp libSDL3.a $(WEB_FLAGS) -o $(WEB_OUT)

run_web: build_web
	emrun $(WEB_OUT)

clean:
	rm -f $(NATIVE_BIN)
	rm -rf docs
