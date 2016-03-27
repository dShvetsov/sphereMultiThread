
#include <iostream>
#include <string>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <strings.h>
#include <sys/epoll.h>
#include <system_error>
#include <errno.h>
#include <ostream>

static const size_t MAX_BUF_CHAT = 1024;

class InputHandler;

class ServConn{
	int sock;
	char buf[MAX_BUF_CHAT + 1];
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
	char buf[MAX_BUF_CHAT + 1];
	int n;
	InputHandler() {};
	void read(){
		n = ::read(In, buf, MAX_BUF_CHAT);
		if (n == 0) { 
			close(In);
		} else {
			buf[n] = '\0';
		}
	}
};
