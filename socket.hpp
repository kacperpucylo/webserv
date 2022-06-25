#ifndef SOCKET_H
#define SOCKET_H

#include "webserv.hpp"

class Socket{
	protected:
		int _sockfd;
		sockaddr_in _sockaddr;

	public:
		Socket();
		Socket(Socket const &other);
		virtual~Socket();

		Socket &operator=(Socket const &other);

		int	socket_fd() const;
		int accept_request() const;
		int s_listen() const;
		virtual int handle_request(int connection) const = 0;
};


//class demostrating inheritance of abstract class Socket
class dummy_socket : virtual public Socket{
	
	public:
		dummy_socket(){}
		dummy_socket(dummy_socket const &other){
			_sockaddr = other._sockaddr;
			_sockfd = other._sockfd;
		};
		virtual~dummy_socket(){}

		dummy_socket &operator=(dummy_socket const &other){
			_sockaddr = other._sockaddr;
			_sockfd = other._sockfd;
			return *this;
		}

		int handle_request(int connection) const {
			// Handle request 
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
				close(connection);
				return 0;
		}
};

#endif
