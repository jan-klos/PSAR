#include <sstream>
#include <vector>
#include <Utils.hpp>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iterator>
#include <openssl/md5.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <openssl/md5.h>

using namespace std;
const int MAX_FILE_PATH = 256;

string exec(const char* cmd)
{
	array<char, 128> buffer;
	string result;
	shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
	if (!pipe) throw runtime_error("popen() failed!");
	while (!feof(pipe.get()))
	{
		if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
			result += buffer.data();
	}
	return result;
}

vector<string> split(const string &s, char delim) 
{
	vector<string> elems;
	stringstream ss(s);
	string item;
	while (getline(ss, item, delim))
	{
		elems.push_back(item);
	}
	return elems;
}

//TODO Properly C++ this
void mac_addr_n2a(string &mac_addr, unsigned char *arg)
{
	int i, l;
	char buff[20];

	l = 0;
	for (i = 0; i < 6 ; i++) 
	{
		if (i == 0) 
		{
			sprintf(buff+l, "%02x", arg[i]);
			l += 2;
		} 
		else 
		{
			sprintf(buff+l, ":%02x", arg[i]);
			l += 3;
		}
	}
	mac_addr.assign(buff);
}

void get_filepath(const char* FILES_DIR, const char* filename, char* filepath)
{
    strcpy(filepath, FILES_DIR);
    strcat(filepath, filename);
}

void convert_file_to_bytes(const char* FILES_DIR, const char* filename, char* content)  
{  
    char* filepath = new char[MAX_FILE_PATH]();
    get_filepath(FILES_DIR, filename, filepath);
    ifstream fl(filepath);  
    fl.seekg(0, ios::end);  
    size_t len = fl.tellg();    
    fl.seekg(0, ios::beg);   
    fl.read(content, len);  
    fl.close();  
    delete [] filepath; filepath = NULL;
}  

vector<string> split_string(string str)
{
    istringstream buf(str);
    istream_iterator<string> beg(buf), end;
    vector<string> tokens(beg, end); 
    return tokens;
}

void create_file(const char *FILES_DIR, string str)
{
	vector<string> msg = split_string(str);
	ofstream file;
	file.open(FILES_DIR + msg[1]); 
	int i;
	for(i = 2; i < msg.size(); i++)
	{
		file << msg[i];
		if(i != msg.size() -1)
		{
			file << " ";
		}
	}
	printf("Created file %s\n", msg[1].c_str());
	file.close();
}

void create_file_sw(const char *FILES_DIR, string str, struct sw_struct* s)
{
	vector<string> msg = split_string(str);
	ofstream file;
	file.open(FILES_DIR + msg[1]); 
	s->filename = &(msg[1])[0u];
	s->send_addr = &(msg[2])[0u];
	s->n = atoi(&(msg[3])[0u]);
	int i;
	for(i = 4; i < msg.size(); i++)
	{
		file << msg[i];
		if(i != msg.size() -1)
		{
			file << " ";
		}
	}
	printf("Created file %s\n", msg[1].c_str());
	
	file.close();
}

void get_file_hash(const char* FILES_DIR, const char* filename, char* hash)
{
	struct stat statbuf;
	char* buf;
	char* filepath = new char[MAX_FILE_PATH]();

	get_filepath(FILES_DIR, filename, filepath);
	int fd = open(filepath, O_RDONLY);
	if(fstat(fd, &statbuf) < 0) 
	{
		return;
	}
	buf = (char*)mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0);
	MD5((unsigned char*) buf, statbuf.st_size, (unsigned char*)hash);
    	munmap(buf, statbuf.st_size); 
	delete [] filepath; filepath = NULL;
}
