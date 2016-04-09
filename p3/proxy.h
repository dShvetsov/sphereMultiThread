#include <fstream>
#include <boost/asio.hpp>
#include <string>
#include <cstdio>
#include <boost/bind.hpp>
#include <memory>
#include <iostream>
#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <map>
#include <cstdint>
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
		int get_port() const {return proxyport;}
		std::vector<dst_struct> get_dst() const {return dst;}
};


class ClientManager;
class Client;
typedef std::shared_ptr<Client> client_ptr;

class Client : public std::enable_shared_from_this<Client> {
	uint64_t cookie;
	ip::tcp::socket sock_;
	ip::tcp::socket serv_sock;
	char client_buffer[1024];
	char server_buffer[1024];
	ip::address server_addr;
	int server_port;
	Client (uint64_t cookie_, ip::address addr, int port) 
	: sock_(service), cookie(cookie_)
	, server_addr(addr), server_port(port), serv_sock(service) { }
	void readfromcli();
	void readcliend(const boost::system::error_code err, size_t size);
	void readfromsrv();
	void readsrvend(const boost::system::error_code err, size_t size);
	void write2cli(char * buf, size_t size);
	void write2srv(char *buf, size_t size);
	void write2cliend(const boost::system::error_code err, size_t size);
	bool started = false;
	void connectend(const boost::system::error_code err);
	void write2srvend(const boost::system::error_code err, size_t size);
public:
	ip::tcp::socket &socket() { return sock_;}
	void start ();
	void stop ();
	friend class ClientManager;
};




class ClientManager {
	std::vector<dst_struct> servers;
	std::map <uint64_t, client_ptr> clients;
	uint64_t next_cookie = 1;
	std::default_random_engine generator;
	std::uniform_int_distribution<int> distribution;
public :
	void set_servers(std::vector<dst_struct> servers_) {
		servers = servers_;
		std::uniform_int_distribution<int> helper(0, servers.size() - 1);
		distribution.param(helper.param());
	}
	client_ptr new_client( );
	void stop(uint64_t cookie){
		clients.erase(cookie);
	}

};
