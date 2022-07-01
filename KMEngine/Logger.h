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

	ofstream KMELog;
};
