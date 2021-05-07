#include <ImpGears/Descriptors/JsonImageOp.h>

#include <iostream>

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


