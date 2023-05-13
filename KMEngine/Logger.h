#pragma once

#include <fstream>
#include <string.h>

using namespace std;

class Logger
{
private:
	Logger();

public:
	static Logger& GetLogger();

	void Log();

	void Log(string Output);

	void Log(string Output, float Value);

	template <typename... Args>
	void Log(Args...args)
	{
		(KMELog << ... << args);
		KMELog << endl;
	}

	ofstream KMELog;
};