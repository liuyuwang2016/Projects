#include <iostream>  
#include <sstream>  
#include <fstream>  
#include <string>  
#include "stdlib.h"  

using namespace std;
int main(int argc, char *argv[]){

	if (argc < 2) {
		cout << "Usage: " << argv[0] << "filename" << endl << endl;
		return -1;
	}

	// define the file to be read  
	ifstream fin(argv[1], ios::in);
	// set the buffer size  
	char line[1024] = { 0 };

	int i = 1;
	// read each line  
	while (fin.getline(line, sizeof(line))){
		// set the current line to a stringstream  
		stringstream ss(line);

		// split the current line into double values  
		cout << "The points in the " << i << "th line are: " << endl;
		while (!ss.eof()) {
			double p[3];
			string token;
			for (unsigned dim = 0; dim < 3; dim++){
				ss >> token;
				p[dim] = atof(token.c_str());
			}
			cout << "<" << p[0] << " " << p[1] << " " << p[2] << ">" << endl;
		}
		cout << endl;
		i++;
	}

	fin.close();

	return 0;
}