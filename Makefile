COMPILER = clang++
OPTIONS = -Wpedantic --std=c++11

all: fbtest

fbtest: fbtest.cpp framebuffer.hpp
	$(COMPILER) $(OPTIONS) $< -o $@

bmpread: bmpread.cpp framebuffer.hpp
	$(COMPILER) $(OPTIONS) $< -o $@
