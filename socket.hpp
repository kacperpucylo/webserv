#ifndef SOCKET_H
#define SOCKET_H

#include "webserv.hpp"

class Socket{
	private:
		int _sockfd;
		sockaddr_in _sockaddr;

	public:
		Socket();
		Socket(Socket const &other);
		~Socket();

		Socket &operator=(Socket const &other);

		int socket_listen();
};

#endif
