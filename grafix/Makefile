ifeq ($(OS),Windows_NT)
	EXE = .exe
	CXX ?= mingw32-g++
	LDLIBS = -lglfw3 -lgdi32 -lopengl32
	CFLAGS += -Ivendor -g -std=c++17
else
	EXE = 
	CXX ?= g++
	LDLIBS = -lglfw -ldl -libfmt
	LDFLAGS += -fsanitize=address,undefined
	CFLAGS += -Ivendor -g -std=c++17 -fsanitize=address,undefined
endif



SRCS = main.cpp vendor/glad/glad.c
HDRS = player.h polygon.h vector2.h collision.h utils.h

grafix$(EXE): $(SRCS) $(HDRS)
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@ $(SRCS) $(LDLIBS)

.PHONY: run
run: grafix$(EXE)
	./$<

polygon$(EXE): polygon.cpp
	$(CXX) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

runPolygon: polygon$(EXE)
	.\$<

.PHONY: clean
clean:
	rm -f grafix$(EXE)
