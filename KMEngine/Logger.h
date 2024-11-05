#pragma once

#include <fstream>
#include <string.h>

using namespace std;

class CLogger
{
private:
	CLogger();

public:
	static CLogger& GetLogger();

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