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

void Logger::Log(string Output, float Value)
{
	KMELog << Output << Value << endl;
}

void Logger::Log()
{
	KMELog << "Oi";
	KMELog << "Oi" << std::flush;
}

//template <class T>
//void Logger::LogArgs(T t)
//{
//	KMELog << t <<", ";
//}

//template <typename... Args>
//void Logger::TemplatedLog(string Text, Args...args)
//{
//	KMELog << Text;
//	LogArgs(args...);
//}