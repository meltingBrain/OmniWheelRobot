#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class WriteCSV{
	private:
		string file_path;
		ofstream written_csv;
		
	public :
		void setFilePath(string wpath){file_path = wpath;}
		void openCSV(){written_csv.open(file_path, ios_base::out);} //ios_base::app : add data from last cell
		void popenCSV(){for(int i=0;i<100;i++)printf("WRITE WRITE WRITE WRITE WRITE WRITE WRITE WRITE\n");}//
		void closeCSV(){written_csv.close();}
		
		inline void writeHorizon(float wval){written_csv << wval <<',';}
		inline void writeVertical(float wval){written_csv << wval << '\n';}
};
