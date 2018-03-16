COMPILER = clang++
OPTIONS = -Wpedantic --std=c++11 -O2

all: fbtest

fbtest: fbtest.cpp framebuffer.hpp
	$(COMPILER) $(OPTIONS) $< -o $@

bmpread: bmpread.cpp framebuffer.hpp
	$(COMPILER) $(OPTIONS) $< -o $@

clean:
	rm -f fbtest bmpread
