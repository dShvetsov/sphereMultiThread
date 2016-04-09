#include "proxy.h"
#include <iostream>

void Config::parse(){
	fscanf(file, "%d ", &proxyport);
	char *str = (char *) malloc(20);
	int dstport;
	size_t n = 20;
	while (getdelim(&str, &n, ':', file) > 0){
		dst_struct pushed;
		int i;
		for (i = 0; str[i] < '0' or str[i] >'9'; i++);
		int start = i;
		for (; str[i] >='0' and str[i] <= '9' or str[i] == '.'; i++);
		str[i] = '\0';
		pushed.ipaddr = std::string(&gstr[start]);
		getdelim(&str, &n, ',', file);
		pushed.port = atoi(str);
		dst.push_back(pushed);
	}
	free(str);
}
using namespace boost::asio;

//void handle_accept()
 
int main(int argc, char ** argv) {
	if (argc < 2) {std::cout << "specify config file\n"; return 0;}
	Config cnfg(argv[1]);
	std::cout << "port is: "<< cnfg.get_port() << std::endl;
	auto v = cnfg.get_dst();
	for (auto i = v.begin(); i != v.end(); i++){
		std::cout << "dest addr:" << i->ipaddr << ": " << i->port << std::endl;
	}
/*	io_service service;
	ip::tcp::endpoing ep(ip::tcp::v4(), cnfg.get_port());
	ip::tcp::acceptor acc(service, ep);
	ip::tcp::socket sock(service);
	acc.async_accept(sock, boost::bind(handle_accept, sock, _1));
	service.run(); */
}