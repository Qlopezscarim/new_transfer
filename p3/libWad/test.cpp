#include "Wad.h"
#include <cstring>
int main()
{
	//Wad* filer = Wad::loadWad("/home/quintin/Desktop/p3/sample1.wad");
	//std::cout <<"getMagic :"<< filer->getMagic() << " :end\n";
	//std::cout << (*filer).offset_d << "\n";
	//std::cout << (*filer).num_desc << "\n";
	//std::cout << (*filer).isContent("/F/F1") << "\n";
	//std::cout << (*filer).isDirectory("/F/F1") << "\n";

		Wad* testWad = Wad::loadWad("/home/quintin/Desktop/p3/sample1.wad");


        //createFile Test 2, creating file in directory
        std::string testPath = "/Gl/ad/example";

        testWad->createFile(testPath);

        std::vector<std::string> testVector;
        int ret = testWad->getDirectory("/Gl/ad/", &testVector);

		std::cout << "1 " << testWad->isContent(testPath) << "\n";
		std::cout << "2 " << testWad->isDirectory(testPath) << "\n";
		std::cout << "3 " << ret << "\n";
		std::cout << "3 " <<  testVector.size()<< "\n";



        //ASSERT_TRUE(testWad->isContent(testPath));
        //ASSERT_FALSE(testWad->isDirectory(testPath));

        //ASSERT_EQ(ret, 2);
        //ASSERT_EQ(testVector.size(), 2);
		
	return 0;
}
