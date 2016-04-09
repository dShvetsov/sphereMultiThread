#include "proxy.h"


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
		pushed.ipaddr = std::string(&str[start]);
		getdelim(&str, &n, ',', file);
		pushed.port = atoi(str);
		dst.push_back(pushed);
	}
	free(str);
}
using namespace boost::asio;

client_ptr ClientManager::new_client() {
	client_ptr cli(new Client());
	clients.push_back(cli);
	return cli;
}

//ip::tcp::endpoint ep;
//ip::tcp::acceptor acc(service);
std::shared_ptr<ip::tcp::acceptor> acc;

ClientManager clientmanager;


void handle_accept(client_ptr cli, const boost::system::error_code & error) {
	cli->start();
	client_ptr new_client = clientmanager.new_client();
	acc->async_accept(new_client->socket(), boost::bind(handle_accept, new_client, _1));
}
 
void Client::read() {
	sock_.async_receive(buffer(client_buffer),
	               boost::bind(&Client::readend, 
	               shared_from_this(), _1, _2));
}

void Client::readend(boost::system::error_code err, size_t size) {
	client_buffer[size] = '\0';
	std::cout << client_buffer;
	write(size);
	read();
}

void Client::start() {
	read();
	std::cout << "client started\n";
}

void Client::write(size_t size){
	sock_.async_send(buffer(client_buffer, size), 
					boost::bind(&Client::writeend,
						shared_from_this(), _1, _2));
}

void Client::writeend(boost::system::error_code err, size_t size) {};

int main(int argc, char ** argv) {
	if (argc < 2) {std::cout << "specify config file\n"; return 0;}
	Config cnfg(argv[1]);
	ip::tcp::endpoint ep(ip::tcp::v4(), cnfg.get_port());
	acc = std::shared_ptr<ip::tcp::acceptor>(new ip::tcp::acceptor(service, ep));
	//ep.port(cnfg.get_port());
	//ep.address(ip::address_v4::any());

	//acc.bind(ep);
	//acc.open(ep.protocol());
//	ip::tcp::endpoint ep(ip::tcp::v4(), cnfg.get_port());
//	ip::tcp::acceptor acc(service, ep);
	client_ptr client = clientmanager.new_client();
	acc->async_accept(client->socket(), boost::bind(handle_accept, client, _1));
	service.run(); 
}