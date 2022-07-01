#include "Logger.h"

Logger::Logger()
{
	KMELog.open("Log.txt");
}

Logger& Logger::GetLogger()
{
	static Logger GLogger;
	return GLogger;
}

void Logger::Log(string Output)
{
	KMELog << Output << endl;
}

void Logger::Log()
{
	KMELog << "Oi";
	KMELog << "Oi" << std::flush;
}