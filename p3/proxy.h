#include <fstream>
#include <boost/asio.hpp>
#include <string>
#include <cstdio>

struct dst_struct{
		int port;
		std::string ipaddr;
};

class Config {
	const char *filename;
	FILE* file;
	int proxyport;
	std::vector<dst_struct> dst;
	void parse();


	public:
		Config(const char *name):filename(name) {
			file = ::fopen(name, "r");
			parse();
		}
		int get_port(){return proxyport;}
		std::vector<dst_struct> get_dst(){return dst;}
};