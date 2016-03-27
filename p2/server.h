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
};