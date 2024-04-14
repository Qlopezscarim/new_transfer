#include "Wad.h"

void reset_cursor(int fd)
{
	lseek(fd,0, SEEK_SET);
}

void move_cursor(int &fd, int offset)
{
	lseek(fd, offset, SEEK_CUR);
}

node next_node(int &fd) //might have to allocate on stack :/
{
	node new_node;
        char name_buffer[8];
        read(fd,&new_node.offset_n,4);
        read(fd,&new_node.length_n,4);
        read(fd,name_buffer,sizeof(name_buffer));
	new_node.name_n = std::string(name_buffer);
	return new_node;
}

void recursive_file_handler(int fd,node &start,int& to_sub)
{
	std::string end_string = start.get_name() + "_END";
	node new_node = next_node(fd);
	while(new_node.name_n != end_string)
	{
		if(new_node.is_map()) //if it is a map
		{
			for(int i=0;i<10;i++)
            {
                node map_child = next_node(fd);
                new_node.children_n.push_back(map_child);
            }
            start.children_n.push_back(new_node);
            to_sub = to_sub - 11;
		}	 
		else if(new_node.is_named_start()) //if it is the start of another directory
		{
			recursive_file_handler(fd,new_node,to_sub);
			start.children_n.push_back(new_node);
			to_sub--;
		}
		else if(new_node.is_named_end())
		{
			break;
		}
		else //add regular files
		{
			start.children_n.push_back(new_node);
			to_sub--;
			//recursive_file_handler(fd,new_node,to_sub);
		}
		new_node = next_node(fd);
	}
	to_sub--;
	return;// new_node;
}

void recursive_print(node &root, int &num_spaces)
{
	for (int holder = num_spaces;holder>0;holder--)
		{std::cout << " ";}
	std::cout << root.get_name() << "\n";
	if(root.children_n.size() != 0)
	{
		num_spaces++;
	}
	for(node element : root.children_n)
	{
		std::cout << "  ";
		recursive_print(element,num_spaces);
		if(element.name_n == root.children_n[root.children_n.size()-1].name_n)
		{num_spaces--;}
	}
	
}

std::vector<std::string> disect_path(std::string path)
{
	std::stringstream ss(path);
	std::string buffer;
	std::vector<std::string> to_return;
	while(std::getline(ss,buffer,'/'))
	{
		if(!buffer.empty())
		{
			to_return.push_back(buffer);
		}
	}
	return to_return;
}



void initial_setup(int fd,int offset,node &root,int num_desc)
{
	reset_cursor(fd);
	move_cursor(fd,offset);
	//we are now ar the first descriptor
	int iterations = num_desc;
	for(iterations;iterations>0;iterations--)
	{
		node new_node = next_node(fd);
		if(new_node.is_map()) //if it is mapped files
		{
			for(int i=0;i<10;i++)
			{
				node map_child = next_node(fd);
				new_node.children_n.push_back(map_child);
			}
			root.children_n.push_back(new_node);
			iterations = iterations-10;
		}
		else if(new_node.is_named_start())
		{
		recursive_file_handler(fd,new_node,iterations);
		root.children_n.push_back(new_node);
		}
		else
		{
			root.children_n.push_back(new_node);
		}
	}
	int num_spaces = 0;
	//recursive_print(root,num_spaces);
}

void get_lump_length(node& root,size_t& net_length)
{
	net_length = net_length + root.length_n;
	for(node element : root.children_n)
	{
		get_lump_length(element,net_length);
	}
}

//pass a file descriptor, offset,ROOT node.num_desc
void move_file(int &fd,uint32_t offset,size_t length_of_insert,node& root, int num_desc)
{
	size_t total_size = 12 /*Size of header*/+  num_desc*16; //and size of descriptors
	get_lump_length(root,total_size);
	size_t size_of_buffer = total_size - offset;
	char* buffer = new char[size_of_buffer];
	reset_cursor(fd);
	move_cursor(fd,offset);
	read(fd,buffer,size_of_buffer);
	//we have stored the data past where we want to insert
	reset_cursor(fd);
	move_cursor(fd,offset+length_of_insert);
	write(fd,buffer,size_of_buffer);
	//there is now free space at offset for length_of_insert
}

void adjust_nodes(node* root,uint32_t offset,size_t added_space)
{
	uint32_t current_length = root->length_n;
	if(root->offset_n > offset) //have to manage shift of lump data
	{
		root->offset_n = root->offset_n + added_space;
	}
	if(!root->children_n.empty())
	{
		for(node& element : root->children_n)
		{
			adjust_nodes(&element,offset,added_space);
		}
	}
	return;
}

//void adjust_file(int fd,)//later

node get_node(const std::string &path, node& root)
{
	std::vector<std::string> order = disect_path(path);
	//bool to_return = false;
	node current_node = root;
	for (std::string name:order)
	{
		for(node child:current_node.children_n)
		{
			if(child.get_name() == name)
			{
				current_node = child;
				break;
			}
		}
	}
	return current_node;
}

node* get_node_ref(const std::string &path, node* root) //THIS IS MISERABLE TO THINK ABOUT
{
	std::vector<std::string> order = disect_path(path);
	//bool to_return = false;
	node* current_node = root;
	for (std::string name:order)
	{
		for(node& child:current_node->children_n)
		{
			if(child.get_name() == name)
			{
				current_node = &child;
				break;
			}
		}
	}
	return current_node;
}

void reset_descriptor(node& root,int& fd,int& iteration) //MUST PASS A 0 AND CURSOR AT DESC
{
	if(iteration == 0) //the root lacks a file descriptor
	{
		iteration++;
		for(node& element : root.children_n)
		{
			reset_descriptor(element,fd,iteration);
		}	
	}
	else
	{
		write(fd,&(root.offset_n),4);
		write(fd,&(root.length_n),4);
		const char* buffer = root.name_n.c_str();
		write(fd,buffer,8);
		for(node& element : root.children_n)
		{
			reset_descriptor(element,fd,iteration);
			//If start folder and element is last
			if(root.is_named_start() && element.name_n == root.children_n.back().name_n )
			{
				node to_add;
				to_add.name_n = root.get_name() + "_END";
				to_add.offset_n = 0;
				to_add.length_n = 0;
				reset_descriptor(to_add,fd,iteration);
			}
		}
		if(root.is_named_start() && root.children_n.empty()) //Handling empty directories
		{
			node to_add;
			to_add.name_n = root.get_name() + "_END";
			to_add.offset_n = 0;
			to_add.length_n = 0;
			reset_descriptor(to_add,fd,iteration);
		}
	}
	return;
}


Wad::Wad(const std::string &path)
{
	//saving file and name buffer
	fd = open(path.c_str(),O_RDWR);
	char buffer[4];
	read(fd,buffer,4);
	//end of saving file and name buffer

	//storing file header
	magic = std::string(buffer);
	read(fd,&num_desc,4);
	std::cout << num_desc;
	read(fd,&offset_d,4);
	//finished storing file header

	//setting up empty root file directory
	root.name_n = "/";
	root.offset_n = 0;
	root.length_n = 0;
	//end of setting up file directory
	initial_setup(fd,offset_d,root,num_desc);

}
Wad* Wad::loadWad(const std::string &path)
{
	Wad* singleton = new Wad(path);
	return singleton;
}
std::string Wad::getMagic()
{
	return magic;
}
bool    Wad::isContent(const std::string &path)
{
	std::vector<std::string> order = disect_path(path);
	//bool to_return = false;
	node current_node = root;
	//int test = 0;
	//recursive_print(root,test);
	if(path.size() == 0 || path.at(0) != '/')
	{return false;}
	for (std::string name:order)
	{
		for(node child:current_node.children_n)
		{
			if(child.get_name() == name)
			{
				current_node = child;
				break;
			}//I know disgusting else if: checking if last child and doesn't match
			else if(child.get_name() != name && child.name_n == current_node.children_n[current_node.children_n.size()-1].name_n)
			{
				return false;
			}
		}
	}
	return !(current_node.is_dir());
}
bool	Wad::isDirectory(const std::string &path)
{
	std::vector<std::string> order = disect_path(path);
	node current_node = root;
	if(path.size() == 0 || path.at(0) != '/')
	{return false;}
	for (std::string name:order)
	{
		for(node child:current_node.children_n)
		{
			if(child.get_name() == name)
			{
				current_node = child;
				break;
			}//I know disgusting else if: checking if last child and doesn't match
			else if(child.get_name() != name && child.name_n == current_node.children_n[current_node.children_n.size()-1].name_n)
			{
				return false;
			}
		}
	}
	return current_node.is_dir();
	//return !(this->isContent(path)); Have to make sure it exist first :/
}
int	Wad::getSize(const std::string &path)
{
	print_everythin();
	if(isContent(path))
	{
		node to_return = get_node(path,root);
		return to_return.length_n;
	}
	else
	{
		return -1;
	}
}
int32_t     Wad::getContents(const std::string &path,char *buffer, int length, int offset)
{
	if(isContent(path))
	{
		node selected = get_node(path,root);
		int32_t left_of_file = selected.length_n - offset;
		if(left_of_file < 0)
		{return 0;} //We went past the file
		else if(left_of_file < length)
		{length = left_of_file;}//WE OBVIOUSLY DON'T WANT TO READ INTO THE NEXT FILE
		reset_cursor(fd);
		move_cursor(fd,selected.offset_n + offset);
		int32_t copied = read(fd,buffer,length);
		return copied;
	}
	else
	{
		return -1;
	}
}
int     Wad::getDirectory(const std::string &path, std::vector<std::string> *directory)
{
	if(isDirectory(path))
	{
		node dir_node = get_node(path,root);
		std::vector<std::string> to_make;
		int size = 0;
		for(node element:dir_node.children_n)
		{
			(*directory).push_back(element.get_name());
			size++;
		}
		return size;
	}
	else
	{
		return -1;
	}
}
void    Wad::createDirectory(const std::string &path)
{
	//light filtering
	size_t lastSlashP = path.find_last_of('/');
	std::string path_filtered;
	std::string new_name;
	if(lastSlashP != path.size()-1 )
    {	path_filtered = path.substr(0, lastSlashP + 1); // Include the last "/"
    	new_name = path.substr(lastSlashP + 1); // Start from the character after the last "/"
	}
	else
	{
		std::string path_2 = path.substr(0,path.size()-1);
		size_t lastSlashP = path_2.find_last_of('/');
		path_filtered = path_2.substr(0, lastSlashP + 1); // Include the last "/"
    	new_name = path_2.substr(lastSlashP + 1); // Start from the character after the last "/"
	}

	if(isDirectory(path_filtered))
	{
		node directory = get_node(path_filtered,root);
		if(directory.is_map())
		{return;}//we do not modify maps!
		if(new_name.size()>2)
		{return;}//We only have so much room!
		node* deep_copy = get_node_ref(path_filtered,&root);
		node new_file; //need to reserve this bye tho no?
		new_file.name_n = new_name + "_START";
		new_file.offset_n = 0;
		new_file.length_n = 0;
		deep_copy->children_n.insert(deep_copy->children_n.begin() + deep_copy->children_n.size(),
		new_file);
		this -> num_desc = this -> num_desc + 2;
		reset_cursor(fd);
		move_cursor(fd,4);
		write(fd,&num_desc,4);
		//Required conditions to call reset_descriptor
		reset_cursor(fd);
		move_cursor(fd,offset_d);
		int zero = 0;
		//End of required conditions
		reset_descriptor(root,fd,zero);
		int test = 0;
		//recursive_print(root,test);
	}
	else
	{return;}

}
void    Wad::createFile(const std::string &path)
{
	//light filtering
	size_t lastSlashP = path.find_last_of('/');
    std::string path_filtered = path.substr(0, lastSlashP + 1); // Include the last "/"
    std::string new_name = path.substr(lastSlashP + 1); // Start from the character after the last "/"

	if(isDirectory(path_filtered))
	{
		node directory = get_node(path_filtered,root);
		if(directory.is_map())
		{return;}//we do not modify maps!
		if(new_name.size()>8)
		{return;}//We only have so much room!
		node* deep_copy = get_node_ref(path_filtered,&root);
		node new_file; //need to reserve this bye tho no?
		new_file.name_n = new_name;
		new_file.offset_n = uint32_t(12);//after the header?
		new_file.length_n = 0;
		if(new_file.is_map())
		{return;} //We don't make maps either
		deep_copy->children_n.insert(deep_copy->children_n.begin() + deep_copy->children_n.size(),
		new_file);
		this -> num_desc++;
		reset_cursor(fd);
		move_cursor(fd,4);
		write(fd,&num_desc,4);
		//Required conditions to call reset_descriptor
		reset_cursor(fd);
		move_cursor(fd,offset_d);
		int zero = 0;
		//End of required conditions
		reset_descriptor(root,fd,zero);
		int test = 0;
		//recursive_print(root,test);
	}
	else
	{return;}
}
int     Wad::writeToFile(const std::string &path, const char* buffer, int length, int offset)
{
	if(!isContent(path))
	{
		return -1;
	}
	else
	{
		//Task: Update header desc offset,update node length, update lump
		//update descriptor offsets
		node* add_to = get_node_ref(path,&root);
		if(add_to->length_n != 0)
		{
			return 0; //already exists
		}
		int orig_file_loc = add_to->offset_n + offset; //the Byte where we need to move file
		move_file(fd,orig_file_loc,length,root,num_desc);
		adjust_nodes(&root,orig_file_loc,length); //Update node length + descriptor offsets
		add_to->set_length(add_to->get_length() + uint32_t(length));

		//CALL UPDARE DESCRIPTORS OR SOMETHING LIKE THAT

		//at this point we have made space where we plan to add things and altered 
		//and where to find lump data
		//std::cout << "We get to here";
		offset_d = offset_d + length; //update header desc offset
		//also need to update header on the actual file
		reset_cursor(fd);
		move_cursor(fd,8);
		write(fd,&offset_d,4);
		//updating file
		reset_cursor(fd);
		move_cursor(fd,offset_d);
		int iteration = 0;
		reset_descriptor(root,fd,iteration);
		//finished updating file header
		reset_cursor(fd);
		move_cursor(fd,orig_file_loc);
		int bytes_written = write(fd,buffer,length); //update lump
		return bytes_written;
		return 0;
	}

}

void 	Wad::print_everythin()
	{
		int test = 0;
		//recursive_print(root,test);
	}
