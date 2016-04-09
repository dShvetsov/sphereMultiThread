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
		for (i = 0; str[i] < '0' or str[i] >'9'; i++);
		pushed.port = atoi(&str[i]);
		dst.push_back(pushed);
	}
	free(str);
}
using namespace boost::asio;

client_ptr ClientManager::new_client() {
	int num = distribution(generator);
	std::cout << num << ' ' << servers[num].ipaddr << ' ' << servers[num].port << std::endl;
	ip::address addr = ip::address::from_string(servers[num].ipaddr);
	int port = servers[num].port;
	client_ptr cli(new Client(next_cookie, addr, port));
	clients[next_cookie] = cli;
	next_cookie++;
	return cli; 
}

std::shared_ptr<ip::tcp::acceptor> acc;

ClientManager clientmanager;

void handle_accept(client_ptr cli, const boost::system::error_code & error) {
	cli->start();
	client_ptr new_client = clientmanager.new_client();
	acc->async_accept(new_client->socket(), boost::bind(handle_accept, new_client, _1));
}
 
void Client::readfromcli() {
	sock_.async_receive(buffer(client_buffer, 1024),
	               boost::bind(&Client::readcliend, 
	               shared_from_this(), _1, _2));
}

void Client::readfromsrv() {
	serv_sock.async_receive(buffer(server_buffer, 1024), 
							boost::bind(&Client::readsrvend,
								shared_from_this(), _1, _2));
}

void Client::readsrvend(const boost::system::error_code err, size_t size) {
	server_buffer[size] = '\0';
	std::cout <<server_buffer;
	if (err) { 
		if (client_alive){
			server_alive = false;
		} else {
			stop();
		}
	} else {
		write2cli(server_buffer, size);
		readfromsrv();
	}
}

void Client::readcliend(const boost::system::error_code err, size_t size) {
	client_buffer[size] = '\0';
	std::cout << client_buffer;
	if (err) {
		if (server_alive){
			client_alive = false;
		} else {
			stop();
		}
	} else {
		write2srv(client_buffer, size);
		readfromcli();
	}
}

void Client::start() {
	ip::tcp::endpoint ep(server_addr, server_port);
	serv_sock.async_connect(ep, boost::bind(&Client::connectend,
											shared_from_this(), _1));
	started = true;
	client_alive = true;
	std::cout << "client started\n";
}

void Client::write2cli(char *buf, size_t size){
	sock_.async_send(buffer(buf, size), 
					boost::bind(&Client::write2cliend,
						shared_from_this(), _1, _2));
}

void Client::write2srv(char *buf, size_t size) { 
	serv_sock.async_send(buffer(buf, size),
						boost::bind(&Client::write2srvend, 
							shared_from_this(), _1, _2)
			);
}

void Client::write2srvend(const boost::system::error_code err, size_t size) {
	if (!client_alive and !server_alive) {
		stop();
	}
};
void Client::write2cliend(const boost::system::error_code err, size_t size) {
	if (!client_alive and !server_alive) {
		stop();
	}
};

void Client::stop(){
	if (started) {
		sock_.close();
		serv_sock.close();
	}
	clientmanager.stop(cookie);
	std::cout << "stopped\n";
}

void Client::connectend(const boost::system::error_code err) {
	if (err) {
		std::cout << "error with connecting server";
		stop();
	}
	server_alive = true;
	readfromsrv();
	readfromcli();
}

int main(int argc, char ** argv) {
	if (argc < 2) {std::cout << "specify config file\n"; return 0;}
	Config cnfg(argv[1]);
	clientmanager.set_servers(cnfg.get_dst());
	ip::tcp::endpoint ep(ip::tcp::v4(), cnfg.get_port());
	acc = std::shared_ptr<ip::tcp::acceptor>(new ip::tcp::acceptor(service, ep));
	client_ptr client = clientmanager.new_client();
	acc->async_accept(client->socket(), boost::bind(handle_accept, client, _1));
	service.run(); 
}