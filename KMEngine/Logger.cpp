#include "Logger.h"

CLogger::CLogger()
{
	KMELog.open("Log.txt");
}

CLogger& CLogger::GetLogger()
{
	static CLogger GLogger;
	return GLogger;
}

void CLogger::Log(string Output)
{
	KMELog << Output << endl;
}

void CLogger::Log(string Output, float Value)
{
	KMELog << Output << Value << endl;
}

void CLogger::Log()
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