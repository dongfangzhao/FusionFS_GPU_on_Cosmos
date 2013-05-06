// Since the point of this filesystem is to learn FUSE and its
// datastructures, I want to see *everything* that happens related to
// its data structures.  This file contains macros and functions to
// accomplish this.

#include <fstream>
#include <string>
#include <iostream>

#include "spade_log.h"

using namespace std;

ofstream spade_log_file;

int log_open()
{
    
	spade_log_file.open("spade.log");
	
	if (spade_log_file.is_open())
		return 0;
		
	return -1;

}

int log_close() {
	
	spade_log_file.close();
	
	return 0;
	
}

void log_msg(string str)
{
	spade_log_file << str << endl;
}
