#include <iostream>
#include "MemoryManager.h"
#include <list>
#include <vector>
#include <climits>
int bestFit(int size, void* data_structure)
{
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
}

int worstFit(int size, void* data_structure)
{
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
}


int main() //main last thing - have to modify bitmap to account for the dumb reserved stuff smh lmao hahahah :(
{
	//useful for testing best_fit
	/*int to_test[7] = {3,0,10,12,2,20,6};
	void* testing = reinterpret_cast<void*>(to_test);
	std::cout << bestFit(20,testing);
	*/

	//useful for testing wosrtFit
	/*int to_test[7] = {3,0,10,12,2,20,12};
	void* testing = reinterpret_cast<void*>(to_test);
	std::cout << worstFit(1,testing);*/
	



	/*MemoryManager ins = MemoryManager(8,bestFit);
	ins.initialize(50);
	void* mem_to_use = ins.allocate(33);
	ins.free(mem_to_use);
	void* hello = ins.getList();
	delete[] reinterpret_cast<int*>(hello);
	int returned = ins.dumpMemoryMap("garb");
	std::cout << "\nthe returned value from dump_mem " << returned;
	std::cout << "\n";
	bool temp_array[] = {0,0,0,0,0,0,0,0, 0,0,1,1,0,0,1,1, 1,1,1,1,0,0,0,0, 0,0};
	bool* pointer = temp_array;
	void* testing_bit_map = reinterpret_cast<void*>(pointer);*/

	//testing no memory allocation
	/*MemoryManager ins = MemoryManager(8,bestFit);
	void* first_to_delete = ins.allocate(80);
	if(first_to_delete == nullptr)
	{std::cout << "\ngood\n";}*/

	/*MemoryManager ins = MemoryManager(8,bestFit);
	ins.initialize(26);
	void* first_to_delete = ins.allocate(80);
	ins.allocate(16);
	void* second_to_delete = ins.allocate(16);
	ins.allocate(8*6);
	ins.free(first_to_delete);
	ins.free(second_to_delete);
	ins.dumpMemoryMap("please");
	void* thing = ins.getBitmap();
	int* to_delete = reinterpret_cast<int*>(thing);
	delete[] to_delete;
	thing = nullptr;
	to_delete = nullptr;*/




	/*int test = 1;
	if (test == 0)
	{
		ins.initialize(50);
		void* mem_to_use = ins.allocate(33);
		ins.free(mem_to_use);
		ins.allocate(33);
		std::cout << "whatsup bitch";
		ins.getReserved();
	}
	else
	{
		ins.initialize(50);
		ins.allocate(1);
		void* mem_to_use = ins.allocate(1);
		ins.allocate(1);
		ins.free(mem_to_use);
		std::cout << "whatsup bitch";
		ins.getReserved();
	}*/
	return 0;
}
