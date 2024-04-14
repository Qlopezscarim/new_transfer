all:
	g++ -o Wad Wad.cpp test.cpp
libWad.a: libWad.o
	ar rcs libWad.a libWad.o
libWad.o: Wad.cpp Wad.h
	c++ -c Wad.cpp -o libWad.o