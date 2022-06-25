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

		int	socket_fd() const;
		int accept_request() const;
		int s_listen() const;
		int handle_request(int connection) const;
};

#endif
