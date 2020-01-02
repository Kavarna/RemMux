#include "RemMux.h"
#include "Logger.h"

int main(int argc, const char *argv[])
{
	try
	{
		Logger::initLogger();
		RemMux::Get()->run(argc, argv);
		RemMux::Destroy();
		Logger::closeLogger();
	}
	catch (const std::exception& e)
	{
		Logger::log("Fatal error: ", e.what());
		Logger::closeLogger();
		std::cout << "Fatal error occured. Check logs.txt for more details.";
		return 1;
	}

	return 0;
}
