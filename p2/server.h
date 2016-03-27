#pragma once


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
#include <map>
#include <sys/ioctl.h> 


static const size_t MAX_BUF_CHAT = 1024;

void  set_nonblock(int );

class Log  {
	std::ostream& out;
public:
	Log (std::ostream& s) : out(s){};
	void say(std::string s) {
		out << s;
	}
};

class Connection {
	int Master;
	std::set<int> fds;
	std::map<int, std::string> msg;
	Log log;
	int epfd;
	struct epoll_event* events;
	const char *welcome = "Welcome to our server!\n";
public:
	Connection(int port = 12345);
	void listen ();
	void  wait ();
	void add_new_user();
	void message(int fd);
	void disconnect(int fd);
	void broadcast(const char *buffer, int len);
	void broadcast(const std::string &s);
};
