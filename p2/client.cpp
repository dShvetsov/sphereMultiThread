#include "client.h"

ServConn::ServConn(){
	sock = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == -1){
		throw std::system_error(errno, std::system_category());
	}
	struct sockaddr_in SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(3100);
	SockAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

	::connect(sock, (const struct sockaddr*)&SockAddr, sizeof(SockAddr));
}

void ServConn::send_msg(const char *buf, int len){
	::send(sock, buf, len, MSG_NOSIGNAL);
}

void ServConn::recive(){
	int N = ::recv(sock, buf, 1024, MSG_NOSIGNAL);
	if (N == -1){
		throw std::system_error(errno, std::system_category());		
	}
	buf[N] = '\0';
	std::cout << buf;
}

void ServConn::send_msg(InputHandler &ih){
	send_msg(ih.buf, ih.n);
}


int main() { 
	try{
	ServConn conn;
	int fd = conn.get_fd();
	InputHandler ih;
	fd_set Set;
	while (true){
		FD_ZERO(&Set);
		FD_SET(STDIN_FILENO, &Set);
		FD_SET(fd, &Set);
		int n = std::max(fd, STDIN_FILENO) +  1;
		int rv = select(n, &Set, NULL, NULL, NULL);
		if (rv == -1){
			throw std::system_error(errno, std::system_category());				
		}
		if (FD_ISSET(STDIN_FILENO, &Set)){
			ih.read();
			conn.send_msg(ih);
		} 
		if (FD_ISSET(fd, &Set)) { 
			conn.recive();
		}
	}
	}
	catch(std::system_error& err) { 
		std::cout << err.what () << std::endl;
	}
	return 0;
}