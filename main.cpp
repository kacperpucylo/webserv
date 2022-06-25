#include "socket.hpp"

int main() {
	try
	{
		Socket new_socket;
		if (new_socket.s_listen() == -1){
			std::cout << "failed to establish connection\n";
			exit(EXIT_FAILURE);
		}
		new_socket.s_handle();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		exit(EXIT_FAILURE);
	}

	
}
