#include "socket.hpp"

int socket_handle(Socket const &sock){

	fd_set current_sockets, ready_sockets;
	int curr_socket = sock.socket_fd();
	FD_ZERO(&current_sockets);
	FD_SET(curr_socket, &current_sockets);

	int connection = 0;
	int max_requests_to_check = curr_socket;

	while(1){
		ready_sockets = current_sockets;

		if (select(FD_SETSIZE, &ready_sockets, NULL, NULL, NULL) < 0)
			throw std::runtime_error(strerror(errno));
		for (int i = 0; i <= max_requests_to_check; i++)
		{
			if (FD_ISSET(i, &ready_sockets)){
				if (i == curr_socket){
					if ((connection = sock.accept_request()) == -1)
						throw std::runtime_error("connection failed");
					if (connection > max_requests_to_check)
						max_requests_to_check = connection;
					FD_SET(connection, &current_sockets);
				}
				else{
					if (sock.handle_request(connection) == -1)
						throw std::runtime_error("request handling failed");
					FD_CLR(connection, &current_sockets);
				}
			}
		}
	}
	return 0;
}
