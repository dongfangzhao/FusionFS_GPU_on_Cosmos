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
#include "provenance.h"
// #include "spade_log.h"

using namespace std;

//============ Local buffer ==============
unordered_map<string, vector<string>> local_buffer;

//============= DS for SPADE internal logic ==============

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

//============== processes info =================

int getProcStatus (int pid, Vertex &procVertex);
int getProcCmd (int pid, Vertex &procVertex);

//============== file info ================

int getFileInfo (const char* path, Vertex &fileVertex);

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

ZHTClient spade_zhtClient;

Vertex::Vertex() {
}

FileVertex::FileVertex() {
}

ProcessVertex::ProcessVertex() {
}

int processTimeFormat (string &s_time) {
	for (int i = 0; i < s_time.length(); ++i) {
    	if (s_time[i] == ' ')
      		s_time[i] = '-';
      	else if (s_time[i] == ':')
      		s_time[i] = '=';
      	else if (s_time[i] == '\n')
      		s_time[i] = 'x';
  	}
  	return 0;
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

int spade_readlink(const char *host, const char *path, pid_t pid, int iotime) {


}

int spade_unlink(const char *host, const char *path, pid_t pid) {


}

int spade_symlink(const char *host, const char *from, const char *to, pid_t pid) {



}

int spade_rename(const char *host, const char *from, const char *to, 
	pid_t pid, int link, int iotime) {


}

int spade_link(const char *host, const char *from, const char *to, pid_t pid) {


}

int getFileInfo (const char* path, Vertex &fileVertex) {

	string fullpath(path);
	fileVertex.addAttribute("Filename", fullpath);

	/*struct stat fileStat;

	if (stat(path, &fileStat) < 0)    
    	return -1;

	char s_size[20];
	sprintf(s_size, "%zd", fileStat.st_size);
    fileVertex.addAttribute("Size", s_size);*/

    /*char s_ino[20];
	sprintf(s_ino, "%zd", fileStat.st_ino);
    fileVertex.addAttribute("Inode", s_ino);*/

    /*string s_mtime(ctime(&fileStat.st_mtime));
    processTimeFormat(s_mtime);
    fileVertex.addAttribute("LastModifiedTime", s_mtime);*/

    return 0;
}

int getProcCmd (int pid, Vertex &procVertex) {
	char procpath[100];
	char buf[100];
	FILE *proc;

	sprintf(procpath, "/proc/%d/cmdline", pid);

	proc = fopen(procpath,"r");

	if (!proc)
		return -1;

	log_msg("=====cshou debug: Proc CMD Open Successful\n");

	if (fgets(buf,256,proc) != NULL) {
		procVertex.addAttribute("cmdline", buf);
		return 0;
	}

	return -1;
}

int getProcStatus (int pid, Vertex &procVertex) {
	char procpath[100];
	char buf[100];
	FILE *proc;

	sprintf(procpath, "/proc/%d/status", pid);

	proc = fopen(procpath,"r");

	if (!proc)
		return -1;

    log_msg("=====cshou debug: Proc Status Open Successful\n");
    char value[100];

    fgets(buf,256,proc);
    sscanf(buf, "Name:\t%s", value);
    procVertex.addAttribute("Name", value);
    //printf("%s\n", value);
    //printf("[%s]", buf);

    fgets(buf,256,proc); 
    sscanf(buf,"State:\t%s", value);
    procVertex.addAttribute("State", value);
    //printf("%s\n", value);
    //printf("[%s]", buf);

	return 0;
}

string getInsertString (string key, string value) {
	Package package;
	package.set_virtualpath(key);
	package.set_realfullpath(value);
	package.set_operation(3);
	string str = package.SerializeAsString();
	return str;
}

string getAppendString (string key, string value) {
	Package package;
	package.set_virtualpath(key);
	package.set_realfullpath(value);
	package.set_operation(4);
	string str = package.SerializeAsString();
	return str;
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

void split(const std::string &s, char delim, vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

vector<string> getAllVertex (string result) {
	vector<string> res;
	
	// test
	result = parsePackResult(result);
	
	log_msg("Result after parsing:" + result);
	
	/*
	const char *cstr = result.c_str();
		char * pch;
		const char *dilim = ":";
	
		pch = strtok((char *)cstr, dilim);
	
		while (pch != NULL) {
			string curr(pch);
			curr = parsePackResult(curr);
	
			// cout << "Get vertex: " << curr << endl;
			string loginfo = "Get vertex: " + curr;
			log_msg(loginfo);
	
			res.push_back(curr);
	
			pch = strtok(NULL, dilim);
		}*/
	
	split(result, ':', res);

	return res;
}

string addHost (string host, string suffix) {
	return host + "$" + suffix;
}

// choice = 0 ancestor
// choice = 1 descendant
string generateKey (string host, int choice, string suffix) {
	if (choice == 0) {
		return "A$" + host + "$" + suffix;
	}
	else if (choice == 1) {
		return "D$" + host + "$" + suffix;
	}
	return "";
}

string generateAncestor (string suffix) {
	return "A$" + suffix;
}

string generateDescendant (string suffix) {
	return "D$" + suffix;
}

string generateAncestor (string host, string suffix) {
	return "A$" + host + "$" + suffix;
}

string generateDescendant (string host, string suffix) {
	return "D$" + host + "$" + suffix;
}

string getNewVectorClock (string fileAttr) {

	// find vector clock
	long old;
	string oldVector;
	string vectorQuery = getLookupString(fileAttr);

	if (spade_zhtClient.lookup(vectorQuery, oldVector) == 0) {

		printf("===============Lookup Successful================\n");

		oldVector = parsePackResult(oldVector);
		old = atol(oldVector.c_str());
		old = old + 1;

		string res;
		stringstream strstream;
		strstream << old;
		strstream >> res;

		printf("version number : %ld\n", old);

		return res;
	}

	return "0";
}

string getOldVectorClock (string fileAttr) {
	string oldVector;
	string vectorQuery = getLookupString(fileAttr);
	if (spade_zhtClient.lookup(vectorQuery, oldVector) == 0) {

		oldVector = parsePackResult(oldVector);

		cout << "========== Old version :" << oldVector << endl;

		return oldVector;
	}
	return "0";
}

int zht_update (string key, string append) {

	string res;
	int stat = spade_zhtClient.lookup(getLookupString(key), res);

	if (stat < 0)
		return -1;

	res = parsePackResult(res);
	
	log_msg("cshou debug ==== zht_update ==== " + res);
	
	res += ":" + append;
	
	log_msg("Updated vaule: " + res);
	
	res = getInsertString(key, res);
	stat = spade_zhtClient.insert(res);

	return stat;
}

/*string createNetworkVertex (string from, string to, string file) {
	return "NETWORK$" +
		"From@" + from + ";" +
		"To@" + to + "$" + 
		file;
}

int spade_receivefile(const char *host, const char *local_path, 
	const char *original_path, const char *original_ip) {

	FileVertex fv;
	getFileInfo(path, fv);

	string fakeProc = "PROCESS$" + "Name@FusionFS$" + fv.toString();
	string origin(original_ip);
	string network = createNetworkVertex(origin, host, fv.toString());
}

int spade_sendfile(const char *host, const char *source_path, 
	const char *dist_path, const char *dist_ip) {


}*/

int spade_create(const char *host, const char *path, pid_t pid) {

	FileVertex fv;
	ProcessVertex pv;

	getFileInfo(path, fv);

	getProcStatus((int) pid, pv);
	getProcCmd((int) pid, pv);

	string fileAttr = fv.toString();
	string procAttr = pv.toString();

	fv.addAttribute("Version", "0");
	string fileAttrWithVersion = fv.toString();

	string procCheck = addHost(host, procAttr);

	// insert file vertex -- is sure to create a new vertex
	log_msg("[create] [insert new file vertex]");
	spade_zhtClient.insert(getInsertString(fileAttr, "0"));
	spade_zhtClient.insert(getInsertString(generateAncestor(fileAttrWithVersion), procCheck));
	spade_zhtClient.insert(getInsertString(generateDescendant(fileAttrWithVersion), "#"));

	// to see if the proc vertex exists
	string testProc;
	int procExist = spade_zhtClient.lookup(getLookupString(procCheck), testProc);

	if (procExist < 0) {
		log_msg("[create] [insert new proc vertex]");
		spade_zhtClient.insert(getInsertString(procCheck, "dummy"));
		spade_zhtClient.insert(getInsertString(generateDescendant(procCheck), fileAttrWithVersion));
		spade_zhtClient.insert(getInsertString(generateAncestor(procCheck), "#"));
	}
	else {
		// spade_zhtClient.append(getAppendString(generateDescendant(procCheck), fileAttrWithVersion));
		log_msg("[create] [use existing proc vertex]");
		zht_update(generateDescendant(procCheck), fileAttrWithVersion);
	}

	// sample code to retrieve result
	// don't delete

	// cout << "[In create] Key: " << generateAncestor(fileAttrWithVersion) << endl;
	string loginfo = "[In create] Key: " + generateAncestor(fileAttrWithVersion);
	log_msg(loginfo);

	string res;

	string ls = getLookupString(generateAncestor(fileAttrWithVersion));

	int r2 = spade_zhtClient.lookup(ls, res);

	// unpackage
	getAllVertex(res);

	return 0;
}

// flag = 0 read, flag = 1 write
int spade_push(const char *host, const char *path, pid_t pid, int flag) {

	// before everything, test
	for (unordered_map<string, vector<string>>::iterator it = local_buffer.begin(); it != local_buffer.end(); ++it) {
		log_msg("file_key: " + it->first);
		for (vector<string>::iterator iit = it->second.begin(); iit != it->second.end(); ++iit) {
			log_msg("	file_values: " + *iit);
		}
	}

	FileVertex fv;
	ProcessVertex pv;

	getFileInfo(path, fv);

	getProcStatus((int) pid, pv);
	getProcCmd((int) pid, pv);

	// you can add more info based on what recorded in buffer
	string file_key(path);
	unordered_map<string, vector<string>>::const_iterator file_got = local_buffer.find(file_key);
	int num_op = 0;
	if (file_got != local_buffer.end())
		num_op = file_got->second.size();
	stringstream ss;
	ss << (int)num_op;
	string s_num_op = ss.str();
	pv.addAttribute("Num_of_Opertaions", s_num_op);	// currently only add num of ops

	string fileAttr = fv.toString();
	string procAttr = pv.toString();

	string procCheck = addHost(host, procAttr);

	string testProc;
	int procExist = spade_zhtClient.lookup(getLookupString(procCheck), testProc);

	if (flag == 0) {

		string version = getOldVectorClock(fileAttr);
		fv.addAttribute("Version", version);
		string fileAttrWithVersion = fv.toString();

		log_msg("[read] [use existing file vertex]");
		zht_update(generateDescendant(fileAttrWithVersion), procCheck);

		if (procExist < 0) {
			log_msg("[read] [insert new proc vertex]");
			spade_zhtClient.insert(getInsertString(procCheck, "dummy"));
			spade_zhtClient.insert(getInsertString(generateAncestor(procCheck), fileAttrWithVersion));
			spade_zhtClient.insert(getInsertString(generateDescendant(procCheck), "#"));
		}
		else {
			//log_msg("Checkpoint9");
			// spade_zhtClient.append(getAppendString(generateAncestor(procCheck), fileAttrWithVersion));
			log_msg("[read] [use existing proc vertex]");
			zht_update(generateAncestor(procCheck), fileAttrWithVersion);
		}

		// only for test
		string loginfo = "[In read] Key: " + generateDescendant(fileAttrWithVersion);
		log_msg(loginfo);

		string res;

		string ls = getLookupString(generateDescendant(fileAttrWithVersion));

		int r2 = spade_zhtClient.lookup(ls, res);

		log_msg("[read - the result]" + res);

		// unpackage
		getAllVertex(res);

	}
	else if (flag == 1) {

		string version = getNewVectorClock(fileAttr);
		fv.addAttribute("Version", version);
		string fileAttrWithVersion = fv.toString();

		log_msg("[write] [insert new file vertex]");
		spade_zhtClient.insert(getInsertString(fileAttr, version));
		spade_zhtClient.insert(getInsertString(generateAncestor(fileAttrWithVersion), procCheck));
		spade_zhtClient.insert(getInsertString(generateDescendant(fileAttrWithVersion), "#"));

		if (procExist < 0) {
			log_msg("[write] [insert new proc vertex]");
			spade_zhtClient.insert(getInsertString(procCheck, "dummy"));
			spade_zhtClient.insert(getInsertString(generateDescendant(procCheck), fileAttrWithVersion));
			spade_zhtClient.insert(getInsertString(generateAncestor(procCheck), "#"));
		}
		else {
			// spade_zhtClient.append(getAppendString(generateDescendant(procCheck), fileAttrWithVersion));
			log_msg("[write] [use existing proc vertex]");
			zht_update(generateDescendant(procCheck), fileAttrWithVersion);
		}

		// only for test
		string loginfo = "[In write] Key: " + generateAncestor(fileAttrWithVersion);
		log_msg(loginfo);

		string res;

		string ls = getLookupString(generateAncestor(fileAttrWithVersion));

		int r2 = spade_zhtClient.lookup(ls, res);

		// unpackage
		getAllVertex(res);
	}

	// clean local buffer
	local_buffer.erase(file_key);

	return 0;

}

int spade_read(const char *host, const char *path, pid_t pid, 
	int iotime, int link) {

	// decide if there is an entry for the file and the proc
	string file_key(path);
	stringstream ss;
	ss << (int)pid;
	string pid_key = ss.str();

	unordered_map<string, vector<string>>::iterator file_got = local_buffer.find(file_key);
	// unordered_map<string, vector<string>>::const_iterator proc_got = local_buffer.find(pid_key);

	string ioTime;
	ostringstream convert;
	convert << iotime;
	ioTime = convert.str();

	if (file_got == local_buffer.end()) {
		vector<string> file_value;
		file_value.push_back(pid_key + "#read#" + ioTime);
		pair<string, vector<string>> mypair(file_key, file_value);
		local_buffer.insert(mypair);
	}
	else {
		file_got->second.push_back(pid_key + "#read#" + ioTime);
	}

	return 0;

}

int spade_write(const char *host, const char *path, pid_t pid, 
	int iotime, int link) {

	// decide if there is an entry for the file and the proc
	string file_key(path);
	stringstream ss;
	ss << (int)pid;
	string pid_key = ss.str();

	unordered_map<string, vector<string>>::iterator file_got = local_buffer.find(file_key);
	// unordered_map<string, vector<string>>::const_iterator proc_got = local_buffer.find(pid_key);

	string ioTime;
	ostringstream convert;
	convert << iotime;
	ioTime = convert.str();

	if (file_got == local_buffer.end()) {
		vector<string> file_value;
		file_value.push_back(pid_key + "#write#" + ioTime);
		pair<string, vector<string>> mypair(file_key, file_value);
		local_buffer.insert(mypair);
	}
	else {
		file_got->second.push_back(pid_key + "#write#" + ioTime);
	}

	return 0;

}

// to replace main in real deploymeny
int spade_init () {
	
	string cfgFile("./src/zht/zht.cfg");
	string memberList("./src/zht/spade_neighbor");

	if (spade_zhtClient.initialize(cfgFile, memberList, true) != 0) {
		cout << "Crap! spade_ZHTClient initialization failed, program exits." << endl;
		return -1;
	}
	
	// open log file
	if (log_open() < 0)
		return -1;
	
	return 0;
}

int spade_close() {
	
	log_close();
	
	return 0;
	
}

/*
int main(int argc, char **argv) {
	return 0;
}*/


/*
int main(int argc, char **argv) {

	string cfgFile("../ZHT/zht.cfg");
	string memberList("../ZHT/neighbor");

	if (spade_zhtClient.initialize(cfgFile, memberList, true) != 0) {
		cout << "Crap! spade_ZHTClient initialization failed, program exits." << endl;
		return -1;
	}

	spade_create("localhost", "/mnt/common/cshou/Provenance/provenance/Makefile", 3335);
	// spade_create("localhost", "/mnt/common/cshou/Provenance/provenance/Makefile", 3352);

	// spade_write("localhost", "/mnt/common/cshou/Provenance/provenance/Makefile", 3335, 10, 0);
	spade_write("localhost", "/mnt/common/cshou/Provenance/provenance/Makefile", 3352, 10, 0);

	spade_read("localhost", "/mnt/common/cshou/Provenance/provenance/Makefile", 3335, 10, 0);
	
	spade_write("localhost", "/mnt/common/cshou/Provenance/provenance/Makefile", 3352, 10, 0);
	
	spade_read("localhost", "/mnt/common/cshou/Provenance/provenance/Makefile", 3352, 10, 0);

	return 0;

}*/

