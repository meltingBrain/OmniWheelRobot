#ifndef TEXT_READER__H
#define TEXT_READER_H

#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace txtRead{
	bool readTextLine(std::string *str ,const std::string fpath);
	
	int str2int(std::string str);
	float str2float(std::string str);
	double str2double(std::string str);
}

#endif TEXT_READER_H
