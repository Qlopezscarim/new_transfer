#include "MemoryManager.h"

int bestFit(int size, void* data_structure)
{
	uint16_t* to_find = reinterpret_cast<uint16_t*>(data_structure);
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
	uint16_t* to_find = reinterpret_cast<uint16_t*>(data_structure);
	int num_holes = to_find[0];
	int offset = -1;
	int best_fit = -1;//was a zero
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

MemoryManager::MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator)
{
	word_size = wordSize;
	alloc_inst = allocator;
	managed_mem = nullptr;
	abstracted_mem = nullptr;
	number_of_words = -1;
}

void MemoryManager::initialize(size_t sizeInWords)
{
	if(sizeInWords <= 65536)
	{
		managed_mem = new char[sizeInWords*word_size]; //I am just assuming it is all ints. very very likely wrong
		abstracted_mem = new bool[sizeInWords];
		number_of_words = sizeInWords;

		for(int i = 0;i<sizeInWords;i++)
		{
			abstracted_mem[i] = 0;
		}
	}
	else
	{
		//size was too large
	}
}

void MemoryManager::shutdown()
{
	if(abstracted_mem != nullptr)
	{
		for(int i = 0;i<number_of_words;i++)
		{
			abstracted_mem[i] = 0;
		}
		//delete[] abstracted_mem;
	}
	while(reserved.empty() != 0)
	{
		reserved.erase(reserved.begin());
	}

}

void *MemoryManager::allocate(size_t sizeInBytes)
{

	int estimate = sizeInBytes/word_size; //will return integer number
	int add_to_estimate = 0;
	if(sizeInBytes%word_size == 0)
	{
		add_to_estimate = 0;
	}
	else
	{
		add_to_estimate = 1;
	}
	estimate = estimate + add_to_estimate;
	//We now have the amount of words we need to store. Let's verify it is below the max.
	if(estimate > number_of_words)
	{
		return nullptr;
	}

	void* for_alloc = this->getList();
	if(for_alloc == nullptr) //if no memory has been initialized
	{
		return nullptr;
	}
	int offset = alloc_inst(estimate,for_alloc);
	uint16_t* to_delete = reinterpret_cast<uint16_t*>(for_alloc);
	delete[] to_delete;
	to_delete = nullptr;
	for_alloc = nullptr;


	if(offset != -1)
	{
		//at this point offset is the starting address, and we will alter things to 1 for estimate bits
		reserved.push_back(offset);
		reserved.push_back(estimate);
		//there is no need to preserve estimate here so we will alter it
		//We do however need to preserve offset, so we will make a local copy
		int address_to_alter = offset;
		//std::cout << offset << "\n";
		while(estimate != 0) //this functions assumes the returned offset is the true 0 starting index
		{
			abstracted_mem[address_to_alter] = 1;
			estimate--;
			address_to_alter++;
		}

		//std::cout << offset*word_size << "\n";
		for(int i = 0;i<number_of_words;i++)
		{
			//std::cout<< "address: " << i << " value: " << abstracted_mem[i] << "\n";
		}
		return reinterpret_cast<void*>(reinterpret_cast<char*>(managed_mem) + offset*word_size);
	}
	else
	{
		return nullptr;
	}
}

void MemoryManager::free(void *address)
{
	//std::cout << "\n\n-----------START OF FREE CALL--------------------\n\n";
	//The basic idealogy of this function is if we clear abtracted memory, we don't have to clear real mem//
	//-----we first have to figure out where in abtracted memory to free----//
	int sizeInBytes = reinterpret_cast<char*>(address) - reinterpret_cast<char*>(managed_mem);
	int offset = sizeInBytes/word_size; //will return integer number
	//std::cout << "\n" << offset << "\n";
	//std::cout <<"\n start of reserved list:";
	for(auto& element:reserved)
	{
		//std::cout << " " << element;
	}
	//Now we need to figure out how much space we allocated to this offset
	int size_allocated_for_this_free = 0;
	int address_in_reserved = 0;
	for(int i = 0;i<reserved.size();i+=2)
	{
		if(reserved[i] == offset)
		{
			size_allocated_for_this_free = reserved[i+1];
			address_in_reserved = i;
		}
	}
	//we will now free this memory in our abstracted memory
	while(size_allocated_for_this_free != 0)
	{
		abstracted_mem[offset] = 0;
		offset++;
		size_allocated_for_this_free--;
	}
	reserved.erase(reserved.begin()+address_in_reserved,reserved.begin()+address_in_reserved+2);//non-inclusive delete,hence the 2
	//std::cout << "\n----------post free-ing status-----------------\n";
	//std::cout <<"\n start of reserved list:";
	for(auto& element:reserved)
	{
		//std::cout << " " << element;
	}
	//std::cout << "\nabtracted_mem\n";
	for(int i = 0;i<number_of_words;i++)
	{
		//std::cout<< "address: " << i << " value: " << abstracted_mem[i] << "\n";
	}

	//std::cout << "\n\n-----------------end of free function-----------------\n\n";
}

void MemoryManager::setAllocator(std::function<int(int,void *)> allocator)
{
	alloc_inst = allocator;
}

int MemoryManager::dumpMemoryMap(char *filename)
{
	int file_descriptor;
	file_descriptor = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0777);
	uint16_t* hole_list1 = reinterpret_cast<uint16_t*>(this->getList());
	if(file_descriptor == -1)
	{
		return -1;
	}
	std::string to_write;
	int number_of_holes = hole_list1[0];
	int current_index = 1;
	while(number_of_holes > 0)
	{
		if(to_write.size() == 0) //the first characters do not need the hyphen
		{
			//writing [first_value, second_value]
			to_write = to_write + "[" + std::to_string(hole_list1[current_index]) + ", " + std::to_string(hole_list1[current_index+1]) + "]";
		}
		else
		{
			//writing - [first_value, second_value]
			to_write = to_write + " - [" + std::to_string(hole_list1[current_index]) + ", " + std::to_string(hole_list1[current_index+1]) + "]";
		}
		current_index = current_index + 2;
		number_of_holes--; //went through a hole

	}
	//std::cout << "\n" << "this is the string we made ----- "<<to_write;
	int output_size =  to_write.size(); //can get the size from here as we do not care about null terminator
	const char* to_output = to_write.c_str();
	int written_bytes = write(file_descriptor,to_output,output_size);
	if(written_bytes == -1)
	{
		close(file_descriptor);
		return -1;
	}

	close(file_descriptor);
	delete[] hole_list1;
	return 0;
}

void *MemoryManager::getList()//need to delete this every time it is called 
{
	//NEED TO RETURN NULL PTR WHEN NO MEMORY IS INITIALIZED
	if(number_of_words == -1)
	{
		return nullptr;
	}
	int offset = 0;
	int size = 0;
	std::vector<int> the_list;
	while(offset < number_of_words)
	{
		if(abstracted_mem[offset] == 0 && size == 0) //start of a new hole
		{
			the_list.push_back(offset);
			size++;
			offset++;
		}
		else if(size !=0 && abstracted_mem[offset] == 0) //counting a hole
		{
			size++;
			offset++;
		}
		else if(size!=0 && abstracted_mem[offset] == 1) //end of a hole
		{
			the_list.push_back(size);
			size = 0;
			offset++;
		}
		else if(size==0 && abstracted_mem[offset] == 1)
		{
			offset++;
		}
		else
		{
			//std::cout << "unexpected behavior in create_list function\n";
			//std::cout << "\nthe size value is " <<size << "\n";
			//std::cout << "\nthe actual value is " << abstracted_mem[offset] << "\n";
		}
	}
	if(size !=0)
	{
		the_list.push_back(size); //this handles the case where the end is a hole
	}
	//std::cout << "the generated vector is: ";
	for (auto& element:the_list)
	{
		//std::cout << " " << element;
	}
	//----------------------converting std::vector to array-----------------//
	int number_of_elements = the_list.size();
	int number_of_holes = number_of_elements/2;
	uint16_t* returned_array = new uint16_t[number_of_elements+1];
	returned_array[0] = number_of_holes;
	for(int i = 0;i<number_of_elements; i++)
	{
		returned_array[i+1] = the_list[i];
	}
	//std::cout << "\n-----------get_list() is returning this----------------\n";
	for(int i = 0;i<number_of_elements+1; i++)
	{
		//std::cout << returned_array[i] << " ";
	}
	//std::cout << "\n---------end of get_list()-------------------\n";
	return reinterpret_cast<void*>(returned_array); //THIS IS GARBAGE ALTER THIS PLZ
}

void *MemoryManager::getBitmap() 
{
	int new_size = number_of_words;
	int leftover = 0;
	if(new_size%8 == 0)
	{
		//we have an 8-bit representable thing
	}
	else
	{
		leftover = new_size%8;
		leftover = 8-leftover;
	}
	int size_of_new_bit_array = new_size+leftover; //8-bit representable boolean array
	bool bit_array[size_of_new_bit_array]; //building a bit array, wil convert to unsigned based off bytes, and then subtract ~255 from each to flip bits.
	for(int i = 0;i<size_of_new_bit_array;i++)
	{
		bit_array[i] = 0;
	}
	for(int i = 0; i<number_of_words;i++) //LETS COME BACK TO THIS, SHOULD BE CORRECT CODE BUT WILL DEAL WITH LATER READ ABOVE COMMENT THO IT TELLS YOU WHAT TO DO
	{
		bit_array[i] = abstracted_mem[i];
	}

	//std::cout <<"\n";
	for(int i = 0;i<size_of_new_bit_array;i++)
	{
		//std::cout << bit_array[i];
		if(i%8==7)
		{
			//std::cout << " ";
		}
	}
	//std::cout << "\n";



	int size_of_final_array = (size_of_new_bit_array/8) + 2;
	uint16_t size_data_to_return = (size_of_new_bit_array/8);
	uint8_t first_value = (uint16_t(size_data_to_return<<8))>>8; //forcefull truncate
	uint8_t second_value = uint8_t((size_data_to_return >> 8));
	uint8_t* array_to_return = new uint8_t[size_of_final_array];
	////std::cout << "\nfirst: " << first_value;
	////std::cout << "\nsecond: " << second_value;
	array_to_return[0] = first_value;
	array_to_return[1] = second_value;
	//just gotta flip the rest starting at 2
	int starting_index = 0;

	for(int i = 2;i<size_of_final_array;i++)
	{	
		array_to_return[i] = (bit_array[0 + starting_index*8]<<0) + (bit_array[1 + starting_index*8]<<1) + (bit_array[2 + starting_index*8]<<2) + (bit_array[3 + starting_index*8]<<3) + (bit_array[4 + starting_index*8]<<4) + (bit_array[5 + starting_index*8]<<5) + (bit_array[6 + starting_index*8]<<6) + (bit_array[7 + starting_index*8]<<7);
		starting_index++;
	}

	//std::cout << "\nstart of returned_array: ";
	for(int i = 0;i<size_of_final_array;i++)
	{
		//std::cout << array_to_return[i] << " ";
	}
	//std::cout << "\n";
	return array_to_return;
}

unsigned MemoryManager::getWordSize()
{
	return word_size;
}

void *MemoryManager::getMemoryStart()
{
	return managed_mem;
}

unsigned MemoryManager::getMemoryLimit()
{
	return number_of_words*word_size; //unsigned integer that is the total BYTES not words
}










MemoryManager::~MemoryManager()
{
	if (managed_mem != nullptr)
	{
	char* want_to_delete = reinterpret_cast<char*>(managed_mem);
	delete[] abstracted_mem;
	delete[] want_to_delete;
	want_to_delete = nullptr;
	managed_mem = nullptr;
	abstracted_mem = nullptr;
	}
}
