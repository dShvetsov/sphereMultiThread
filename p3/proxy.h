#include <fstream>
#include <boost/asio.hpp>
#include <string>
#include <cstdio>
#include <boost/bind.hpp>
#include <memory>
#include <iostream>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
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

class Client : public std::enable_shared_from_this<Client> {
	ip::tcp::socket sock_;
	char client_buffer[1024];
	Client () :sock_(service) { }
	void read();
	void readend(boost::system::error_code err, size_t size);
	void write(size_t size);
	void writeend(boost::system::error_code err, size_t size);
public:
	ip::tcp::socket &socket() { return sock_;}
	void start ();
	friend class ClientManager;
};




class ClientManager { 
	std::vector <client_ptr> clients;

public :
	client_ptr new_client( );


};
