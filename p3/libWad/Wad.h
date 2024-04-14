#ifndef WAD_H
#define WAD_H

#include <string>
#include <vector>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sstream>

struct node
        {
                uint32_t offset_n;
                uint32_t length_n = 0;
                std::string name_n;
                std::vector<node> children_n;
	bool is_map()
	{
		bool is = false;
		//abusing short circuit evaluation
		if(name_n.size() > 3 && name_n.at(0) == 'E' && std::isdigit(name_n.at(1)) && name_n.at(2) == 'M' && std::isdigit(name_n.at(3)))
		{
			is = true;
		}
		return is;
	}
	bool is_named_start()
	{
		bool is = false;
		std::string dir_check = "_START";
		size_t found = name_n.find(dir_check);
		if(found != std::string::npos)
		{
			is = true;
		}
		return is;
	}
	bool is_named_end()
        {
                bool is = false;
                std::string dir_check = "_END";
                size_t found = name_n.find(dir_check);
                if(found != std::string::npos)
                {
                        is = true;
                }
                return is;
        }
	bool is_dir()
	{
		return (this->is_map()||this->is_named_start()||this->is_named_end()||this->name_n == "/");
	}
	std::string get_name()
	{
		std::string final_name;
		if(this->is_named_start())
		{
			std::string dir_check = "_START";
			size_t found = name_n.find(dir_check);
			final_name = name_n.substr(0,found);
		}
		else if(this->is_named_end())
		{
			//do nothing
		}
		else
		{
			final_name = name_n;
		}
		return final_name;
	}
	void set_length(uint32_t length)
	{
		length_n = length; 
	}
	uint32_t get_length()
	{
		return length_n;
	}
        };

class Wad {
private:
	Wad(const std::string &path);
public: //edit this out later
	std::string magic;
	uint32_t num_desc;
	uint32_t offset_d;
	int fd;
	node root;
public:
    // shame Constructor
	static Wad* loadWad(const std::string &path);
	std::string getMagic();
	bool	isContent(const std::string &path);
	bool	isDirectory(const std::string &path);
	int	getSize(const std::string &path);
	int	getContents(const std::string &path,char *buffer, int length, int offset = 0);
	int	getDirectory(const std::string &path, std::vector<std::string> *directory);
	void	createDirectory(const std::string &path);
	void	createFile(const std::string &path);
	int	writeToFile(const std::string &path, const char* buffer, int length, int offset = 0);
	void print_everythin(); //-----------------------------------___DELETE_______

    // Destructor
    //~Person();
};

#endif //WADR_H 
