#include "server.h"

Connection::Connection(int port):log(std::cout) { 
	/* This may be error !!!! If Failed pay attention to SockAddr and event!!!!*/
	Master = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in SockAddr;
	bzero(&SockAddr, sizeof(SockAddr));
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(port);
	SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(Master, (struct sockaddr *)&SockAddr, sizeof(SockAddr));
	epfd = epoll_create1(0);
	struct epoll_event event;
	event.data.fd = Master;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, Master, &event);
	events = (epoll_event *)calloc(128, sizeof(struct epoll_event));
	log.say("Server already to work\n");
}

void Connection::listen() { 

	if (::listen(Master, SOMAXCONN) == -1) {
		throw std::system_error(errno, std::system_category());
	}
	log.say("Server start listen\n");
}


void Connection::wait(){
	log.say("Server wait for event\n");
	int N = epoll_wait(epfd, events, 1024, -1);
	if (N == -1){
		throw std::system_error(errno, std::system_category());
	}
	for (unsigned int i = 0 ; i < N; i++) {
			if (events[i].data.fd == Master) { 
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
	fds.push_back(Slave);
	log.say("new user connected\n");
}

void Connection::message(int fd){
	char buf[1024];
	int n = recv(fd, &buf, 1024, MSG_NOSIGNAL);
	buf[n] = '\0';
	log.say(std::string(buf));
}

int main() { 
	Connection conn(12345);
	conn.listen();
	for (;;) { 
		conn.wait();
	}
	return 0;
	/*
	int MasterSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in SockAddr;
	bzero(&SockAddr, sizeof(SockAddr));
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(12345);
	SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(MasterSocket, (struct sockaddr *)&SockAddr, sizeof(SockAddr));
	listen(MasterSocket, SOMAXCONN);
	int SlaveSocket = accept(MasterSocket, 0,0);
	while (true){
		char buf[100];
		recv(SlaveSocket, &buf, 30, MSG_NOSIGNAL);
		std::cout << buf;
	}
	return 0;
	*/
}