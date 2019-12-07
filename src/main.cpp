#include "RemMux.h"
#include <iostream>


int main(int argc, const char *argv[])
{
	try
	{
		RemMux::Get()->run(argc, argv);
		RemMux::Destroy();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << '\n';
		return 1;
	}

	return 0;
}
