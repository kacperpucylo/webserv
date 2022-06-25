#include "socket.hpp"

int main() {
	try
	{
		dummy_socket new_socket;
		if (new_socket.s_listen() == -1){
			std::cout << "failed to establish connection\n";
			exit(EXIT_FAILURE);
		}
		socket_handle(new_socket);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		exit(EXIT_FAILURE);
	}
}
