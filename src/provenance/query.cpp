#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/errno.h>
#include <netdb.h>
#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/procfs.h>
#include <sys/fcntl.h>
#include <vector>
#include <netdb.h>
#include <iostream>
#include <unordered_map>

//#include "cpp_zhtclient.h"
#include "../zht/inc/cpp_zhtclient.h"

using namespace std;

ZHTClient query_zhtClient;

typedef unordered_map <string, string> Map;

class Vertex {
protected:
	Map attributes;
public:
	Vertex();
	virtual string toString() = 0;
	int addAttribute (string key, string value);
};

class FileVertex : public Vertex {
public:
	FileVertex();
	string toString();
};

class ProcessVertex : public Vertex {
public:
	ProcessVertex();
	string toString();
};

Vertex::Vertex() {
}

FileVertex::FileVertex() {
}

ProcessVertex::ProcessVertex() {
}

int Vertex::addAttribute (string key, string value) {
	attributes.insert(Map::value_type(key, value));
	return 0;
}

string FileVertex::toString() {
	string res = "FILE$";
	for ( Map::iterator it = attributes.begin(); it != attributes.end(); ++it ) {
		res += it->first + "@" + it->second + ";";
	}
	return res;
}

string ProcessVertex::toString() {
	string res = "PROCESS$";
	for ( Map::iterator it = attributes.begin(); it != attributes.end(); ++it ) {
		res += it->first + "@" + it->second + ";";
	}
	return res;
}

void split(const std::string &s, char delim, vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

void printHelp () {
	// cout << "Example for querying vertex:" << endl;
	cout << "query vertex [filename] [version] [ancestors | descendants] [depth]" << endl;
}

bool checkFormat (vector<std::string> elems) {

	cout << "Elem first:" << elems.at(0) << endl;
	cout << "Elem 1:" << elems.at(1) << endl;
	cout << "Elem 4:" << elems.at(4) << endl;


	if (elems.size() != 6)
		return false;
	
	if (elems.at(0).compare("query") != 0)
		return false;
	
	if (elems.at(1).compare("vertex") != 0)
		return false;
	
	if (elems.at(4).compare("ancestors") != 0 &&
		elems.at(4).compare("descendants") != 0)
			return false;
	
	// don't care for the last arg now
	return true;
}

string getLookupString (string key) {
	Package package;
	package.set_virtualpath(key);
	package.set_operation(1);
	string str = package.SerializeAsString();
	return str;
}

string parsePackResult (string whole) {
	Package package;
	package.ParseFromString(whole);
	return package.realfullpath();
}

vector<string> getAllVertex (string result) {
	vector<string> res;
	
	result = parsePackResult(result);
	
	split(result, ':', res);

	return res;
}

string generateAncestor (string suffix) {
	return "A$" + suffix;
}

string generateDescendant (string suffix) {
	return "D$" + suffix;
}

// not used
string getFileQueryString (string filename, string version) {
	FileVertex fv;
	fv.addAttribute("Filename", filename);
	fv.addAttribute("Version", version);
	
	string lookUpString = getLookupString(generateDescendant(fv.toString()));
	
	//string res;
	
	//int code = query_zhtClient.lookup(lookUpString, res);
	
}

void queryRawDescendants (string vertex) {
	
	cout << vertex << endl;
	
	string lookupString = getLookupString(generateDescendant(vertex));
	string res;
	
	int code = query_zhtClient.lookup(lookupString, res);
	
	if (code < 0)
		return;
	
	vector<string> all = getAllVertex(res);
	
	for (vector<string>::iterator it = all.begin() ; it != all.end(); ++it) {
		queryRawDescendants (*it);
	}
}

void queryRawAncestors (string vertex) {
	
	cout << vertex << endl;
	
	string lookupString = getLookupString(generateAncestor(vertex));
	string res;
	
	int code = query_zhtClient.lookup(lookupString, res);
	
	if (code < 0)
		return;
	
	vector<string> all = getAllVertex(res);
	
	for (vector<string>::iterator it = all.begin() ; it != all.end(); ++it) {
		queryRawAncestors (*it);
	}
}

// so far, there is no duplicate detection
// thus, this function might not be terminated at all
// for now, it's DFS, need to change to BFS later
void queryFileDescendants (string filename, string version) {
	
	FileVertex fv;
	fv.addAttribute("Filename", filename);
	fv.addAttribute("Version", version);
	
	cout << fv.toString() << endl;
	
	string lookupString = getLookupString(generateDescendant(fv.toString()));
	
	string res;
	
	int code = query_zhtClient.lookup(lookupString, res);
	
	if (code < 0)
		return;
	
	vector<string> all = getAllVertex(res);
	
	for (vector<string>::iterator it = all.begin() ; it != all.end(); ++it) {
		queryRawDescendants (*it);
	}

}

void queryFileAncestors (string filename, string version) {
	
	FileVertex fv;
	fv.addAttribute("Filename", filename);
	fv.addAttribute("Version", version);
	
	cout << fv.toString() << endl;
	
	string lookupString = getLookupString(generateAncestor(fv.toString()));
	
	string res;
	
	int code = query_zhtClient.lookup(lookupString, res);
	
	if (code < 0)
		return;
	
	vector<string> all = getAllVertex(res);
	
	for (vector<string>::iterator it = all.begin() ; it != all.end(); ++it) {
		queryRawAncestors (*it);
	}
	
}

void parseInput () {
	
	cout << "Welcome to use Simple Query client... Enjoy!" << endl;
	
	bool shutdown = false;
	
	while (!shutdown) {
		
		string line;
		getline(cin, line);
		
		if (line.compare("help") == 0) {
			printHelp();
		}
		else if (line.compare("exit") == 0) {
			shutdown = true;
		}
		else {
			
			vector<std::string> elems;
			split(line, ' ', elems);
			
			bool isCorrect = checkFormat(elems);
			
			if (!isCorrect) {
				cout << "Wrong query format!" << endl;
				continue;
			}
			
			string filename = elems.at(2);
			string dict = elems.at(4);
			// char* versionNum = elems.at(3).c_str();
			// int version = atoi(versionNum);
			string version = elems.at(3);
			
			if (dict.compare("descendants") == 0) {
				queryFileDescendants(filename, version);
			}
			else {
				queryFileAncestors(filename, version);
			}
			
		}
	}
	
}

int main(int argc, char **argv) {

	char cwd[1024];
       	if (getcwd(cwd, sizeof(cwd)) != NULL)
        	fprintf(stdout, "Current working dir: %s\n", cwd);

	string cfgFile("../zht/zht.cfg");
	string memberList("../zht/spade_neighbor");

	if (query_zhtClient.initialize(cfgFile, memberList, true) != 0) {
		cout << "Crap! spade_ZHTClient initialization failed, program exits." << endl;
		return -1;
	}

	parseInput();

	return 0;

}
