#include <fstream>
#include <boost/asio.hpp>
#include <string>
#include <cstdio>
#include <boost/bind.hpp>
#include <memory>
#include <iostream>
using namespace boost::asio;

io_service service;

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

class ClientManager;
class Client;
typedef std::shared_ptr<Client> client_ptr;

class Client {
	ip::tcp::socket sock_;

	Client () :sock_(service) { }
public:
	ip::tcp::socket &socket() { return sock_;}
	void start () {
		std::cout << "client started\n";
	}
	friend class ClientManager;
};




class ClientManager { 
	std::vector <client_ptr> clients;

public :
	client_ptr new_client( );


};
