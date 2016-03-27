#include "server.h"

Connection::Connection(int port):log(std::cout) { 
	Master = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Master == -1){
		throw std::system_error(errno, std::system_category());
	}
	struct sockaddr_in SockAddr;
	bzero(&SockAddr, sizeof(SockAddr));
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(port);
	SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int optval = 1;
	setsockopt(Master, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	if (bind(Master, (struct sockaddr *)&SockAddr, sizeof(SockAddr)) == -1){
		throw std::system_error(errno, std::system_category());
	}
	epfd = epoll_create1(0);
	if (epfd == -1){
		throw std::system_error(errno, std::system_category());
	}
	struct epoll_event event;
	event.data.fd = Master;
	event.events = EPOLLIN | EPOLLET;
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, Master, &event) == -1){
		throw std::system_error(errno, std::system_category());
	}
	events = (epoll_event *)calloc(128, sizeof(struct epoll_event));
	if (events == NULL) { 
		throw std::system_error(errno, std::system_category());
	}
}

void Connection::listen() { 

	if (::listen(Master, SOMAXCONN) == -1) {
		throw std::system_error(errno, std::system_category());
	}
}


void Connection::wait(){
	int N = epoll_wait(epfd, events, 1024, -1);
	if (N == -1){
		throw std::system_error(errno, std::system_category());
	}
	for (unsigned int i = 0 ; i < N; i++) {
		if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP)) {
			disconnect(events[i].data.fd);
		}else if (events[i].data.fd == Master) { 
			add_new_user();
		} else { 
			message(events[i].data.fd);
		}
	}
}

void Connection::add_new_user(){
	int Slave = accept(Master, 0,0);
	if (Slave == -1) {
		throw std::system_error(errno, std::system_category());
	}
	struct epoll_event event;
	event.data.fd = Slave;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, Slave, &event);
	fds.insert(Slave);
	send(Slave, welcome, 24, MSG_NOSIGNAL);
	log.say("LOG:: accepted connection\n");

}

void Connection::message(int fd){
	char buf[1024];
	int n = recv(fd, &buf, 1024, MSG_NOSIGNAL);
	if (n < 0) { 
		throw std::system_error(errno, std::system_category());
	} else if (n == 0) {
		disconnect(fd);
	} else {
		buf[n] = '\0';
		log.say(std::string(buf));
		broadcast(buf, n);
	}
}

void Connection::disconnect(int fd)  {
	close(fd);
	fds.erase(fd);
	log.say("connection terminated\n");
}

void Connection::broadcast(const char *buffer, int len) {
	for (auto it = fds.begin(); it != fds.end(); it++){
		send(*it, buffer, len, MSG_NOSIGNAL);
	}
}

int main() { 
	try{
	Connection conn(3100);
	conn.listen();
	for (;;) { 
		conn.wait();
	}}
	catch(std::system_error& err) { 
		std::cout << err.what () << std::endl;
	}
	return 0;
}