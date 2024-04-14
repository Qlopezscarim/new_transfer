#all : MemoryManager.cpp MemoryManager.h
#	#c++ -std=c++17 -g -o test_1.o MemoryManager.cpp test.cpp
#	#./test_1.o
libMemoryManager.a: MemoryManager.o
	ar rcs libMemoryManager.a MemoryManager.o
MemoryManager.o: MemoryManager.cpp MemoryManager.h
	g++ -c MemoryManager.cpp -o MemoryManager.o
