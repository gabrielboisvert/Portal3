#include "..\..\..\include\Core\Debug\Log.h"

Core::Log& Core::Log::log = Log::Log();

Core::Log::Log() {}

Core::Log::~Log()
{
	this->closeFile();
}

void Core::Log::closeFile()
{
	if (Log::log.file.is_open())
		Log::log.file.close();
}

void Core::Log::writeToFile(const std::string& str)
{
	if (Log::log.file.is_open())
	{
		Log::log.file.write(str.c_str(), str.length());
		Log::log.file.flush();
	}
}

Core::Log& Core::Log::getInstance()
{
	return Log::log;
}

void Core::Log::openFile(const std::filesystem::path& path)
{
	Log::log.closeFile();
	Log::log.file = std::fstream(path.c_str(), std::fstream::out);
}