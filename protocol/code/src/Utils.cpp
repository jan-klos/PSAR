#include <sstream>

#include <Utils.hpp>

using namespace std;

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