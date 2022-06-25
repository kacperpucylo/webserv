#include "socket.hpp"

int main() {
	try
	{
		Socket new_socket;
		int connection = new_socket.socket_listen();
		// Read from the connection
		char buffer[100];
		ssize_t bytesRead;
		if ((bytesRead= read(connection, buffer, 100)) == -1) {
		perror("read");
		exit(EXIT_FAILURE);
		}
		std::cout << "The message was: " << buffer;

		// Send a message to the connection
		std::string response = "Good talking to you\n";
		send(connection, response.c_str(), response.size(), 0);

		// Close the connections
		close(connection);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		exit(EXIT_FAILURE);
	}

	
}
