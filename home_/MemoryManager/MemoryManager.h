//this is a header file
#pragma once

#include <functional>
#include <vector>
#include <list>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <climits>

int bestFit(int size, void* data_structure);
/*{
	int* to_find = reinterpret_cast<int*>(data_structure);
	int num_holes = to_find[0];
	int offset = -1;
	int best_fit = INT_MAX;
	for(int iterate = 2;iterate<((num_holes*2)+1);iterate+=2)
	{
		int diff = to_find[iterate]-size;
		if(diff >(-1) && diff<best_fit && best_fit != 0) //if the hole is large enough, smaller than previous holes, and a perfect has not already been found
		{
			best_fit = diff;
			offset = to_find[iterate-1];
		}
	}
	return offset;
}*/

int worstFit(int size, void* data_structure);
/*{
	int* to_find = reinterpret_cast<int*>(data_structure);
	int num_holes = to_find[0];
	int offset = -1;
	int best_fit = 0;
	for(int iterate = 2;iterate<((num_holes*2)+1);iterate+=2)
	{
		int diff = to_find[iterate]-size;
		if(diff >(-1) && diff>best_fit && best_fit != diff) //if the hole is large enough, smaller than previous holes, and a perfect has not already been found
		{
			best_fit = diff;
			offset = to_find[iterate-1];
		}
	}
	return offset;
}*/

class MemoryManager{
public:
std::function<int(int,void *)> alloc_inst;
size_t word_size;
bool* abstracted_mem;
void* managed_mem;
long long number_of_words;
std::vector<int> reserved;
public:
MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator);
~MemoryManager();
void initialize(size_t sizeInWords);
void shutdown();
void *allocate(size_t sizeInBytes);
void free(void *address);
void setAllocator(std::function<int(int, void *)> allocator);
int dumpMemoryMap(char *filename);
void *getList();
void *getBitmap();
unsigned getWordSize();
void *getMemoryStart();
unsigned getMemoryLimit();
void getReserved();






};

