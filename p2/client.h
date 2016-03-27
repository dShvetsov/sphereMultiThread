
#include <iostream>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>
#include <strings.h>
#include <set>
#include <sys/epoll.h>
#include <system_error>
#include <errno.h>
#include <ostream>

class InputHandler;

class ServConn{
	int sock;
	char buf[1024];
public:
	ServConn();
	int get_fd(){return sock;}
	void send_msg(const char *buf, int len);
	void send_msg(InputHandler& ih);
	void recive();
};

class InputHandler{
	int In = STDIN_FILENO;
public:
	char buf[1024];
	int n;
	InputHandler() {};
	void read(){
		n = ::read(In, buf, 1024);
		if (n == 0) { 
			close(In);
		} else {
			buf[n] = '\0';
		}
	}
};