#include <Utils/JsonImageOp.h>
// #include <Utils/JsonReaderWriter.h>

// #include <ctime>
#include <iostream>
// #include <fstream>
// #include <map>

// -----------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{	
	if(argc <2)
	{
		std::cout << "no file given" << std::endl;
		return 0;
	}
   
	imp::generateImageFromJson(argv[1]);
   
    return 0;
}


