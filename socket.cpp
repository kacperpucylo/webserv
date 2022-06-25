#include "socket.hpp"

Socket::Socket(){
	if ((_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		throw std::runtime_error("socket fd assignment failed");
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_addr.s_addr = INADDR_ANY;
	_sockaddr.sin_port = htons(4242);
	if (bind(_sockfd, (struct sockaddr*)&_sockaddr, sizeof(sockaddr)) < 0)
		throw std::runtime_error("failed to bind to port 4242");
}
Socket::Socket(Socket const &other) : _sockfd(other._sockfd), _sockaddr(other._sockaddr) {}
Socket::~Socket(){
	close(_sockfd);
}

Socket &Socket::operator=(Socket const &other){
	_sockfd = other._sockfd;
	_sockaddr = other._sockaddr;
	return *this;
}

int Socket::s_listen(){
	if (listen(_sockfd, 10) < 0) {
		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
		return -1;
	}
	return 0;
}

int Socket::s_handle(){

	fd_set current_sockets, ready_sockets;
	FD_ZERO(&current_sockets);
	FD_SET(_sockfd, &current_sockets);

	int connection = 0;

	while(1){
		ready_sockets = current_sockets;

		if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
			throw std::runtime_error(strerror(errno));
		for (int i = 0; i <= 25; i++)
		{
			if (FD_ISSET(i, &ready_sockets)){
				if (i == _sockfd){
					socklen_t addrlen = sizeof(sockaddr);
					connection = accept(_sockfd, (struct sockaddr*)&_sockaddr, (socklen_t*)&addrlen);
					if (connection < 0) {
						std::cout << "Failed to grab connection. errno: " << errno << std::endl;
						return -1;
					}
					FD_SET(connection, &current_sockets);
				}
				else{
					// Read from the connection 
					char buffer[100];
					ssize_t bytesRead;
					bzero(buffer, 100);
					if ((bytesRead= read(connection, buffer, 100)) == -1) {
						perror("read");
						return -1;
					}
					std::cout << "The message was: " << buffer;

					// Send a message to the connection
					std::string response = "Good talking to you\n";
					send(connection, response.c_str(), response.size(), 0);

					// Close the connections
					FD_CLR(connection, &current_sockets);
					close(connection);
				}

			}

		}
		
	}

	
	return 0;
}
