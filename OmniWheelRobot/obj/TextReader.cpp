#include "TextReader.hpp"

bool txtRead::readTextLine(std::string *str ,const std::string fpath) //.txtファイルの読み込み
{
	std::ifstream ifs(fpath);
	if(!ifs){
		printf("Failed to Open .txt File \n");
		return false; 
	}

	std::string line;
	std::vector<std::string> txt_str; 
	while(std::getline(ifs, line))
	{
		txt_str.push_back(line);
	}
	
	for(int i = 0; i < txt_str.size(); i++){
		str[i] = txt_str[i];
	}
	ifs.close();
	
	return true;
}

int txtRead::str2int(std::string str){return std::stoi(str);}
float txtRead::str2float(std::string str){return std::stof(str);}
double txtRead::str2double(std::string str){return std::stod(str);}
