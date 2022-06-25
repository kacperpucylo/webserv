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

int Socket::socket_listen(){
	if (listen(_sockfd, 10) < 0) {
		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
		return -1;
	}

	socklen_t addrlen = sizeof(sockaddr);
	int connection = accept(_sockfd, (struct sockaddr*)&_sockaddr, (socklen_t*)&addrlen);
	if (connection < 0) {
		std::cout << "Failed to grab connection. errno: " << errno << std::endl;
		return -1;
	}
	return connection;
}
