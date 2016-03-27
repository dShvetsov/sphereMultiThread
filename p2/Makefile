
all :chatsrv client
	

client : client.cpp client.h
	g++ client.cpp -o client -std=c++11

chatsrv : server.cpp server.h
	g++ server.cpp -o chatsrv -std=c++11




clean : 
	rm chatsrv -f
	rm client -f