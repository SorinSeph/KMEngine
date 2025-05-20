#pragma once
#include <iomanip>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
#include "Logger.h"

class CImporterGLTF
{
public:
	inline void ImportGLTF()
	{
		CLogger& Logger = CLogger::GetLogger();

		std::ifstream infile(
			"C:/dev/GLTF/Lowpoly_sword/Lowpoly_sword.bin",
			std::ios::in | std::ios::binary
		);
		if (!infile) {
			std::cerr << "Cannot open\n";
			return;
		}

		std::vector<uint8_t> buf{
			std::istreambuf_iterator<char>(infile),
			std::istreambuf_iterator<char>()   // note the ()
		};

		for (int i = 0; i < buf.size() - 3; i += 4)
		{
			float Value;
			std::memcpy(&Value, &buf[i], sizeof(float));
			Logger.Log("Value: " + std::to_string(Value));
		}
	}
};