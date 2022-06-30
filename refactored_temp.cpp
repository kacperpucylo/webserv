#include "webserv.hpp"

#define PORT "4242"

//TBD: inet_ntop() is not allowed by the subject so look for an alternative
//TBD: left off at line 140 in temp.cpp

// get sockaddr
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET)
		return (&(reinterpret_cast<struct sockaddr_in *>(sa)->sin_addr));
	return (&(reinterpret_cast<struct sockaddr_in6 *>(sa)->sin6_addr));
}

int main(void)
{
	fd_set master; // master fd list
	fd_set read_fds; // temp fd list for select()
	int fdmax; // maximum file descriptor number

	int listener; // listening socket descriptor
	int newfd; // newly accepted socket descriptor
	struct sockaddr_storage remoteaddr; // client address
	socklen_t addrlen;

	char buff[256]; // buffer for client data
	int nBytes;

	std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

	char remoteIP[INET6_ADDRSTRLEN];
	int yes = 1; // for setsockopt() SO_REUSEADDR
	int i, j, rv;

	struct addrinfo hints, *ai, *p;

	FD_ZERO(&master); // clear the fd_sets
	FD_ZERO(&read_fds);

	// get a socket and bind it
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	rv = getaddrinfo(NULL, PORT, &hints, &ai);
	if (rv != 0)
	{
		perror("getaddrinfo");
		exit(1);
	}

	for (p = ai; p != NULL; p = p->ai_next)
	{
		listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		fcntl(listener, F_SETFL, O_NONBLOCK);
		if (listener < 0)
			continue ;
		
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0)
		{
			close(listener);
			continue ;
		}

		break ;
	}

	// if we got here, it means we failed to bind
	if (p == NULL)
	{
		std::cerr << "webserv: failed to bind" << std::endl;
		exit(2);
	}

	// listen
	if (listen(listener, 10) == -1)
	{
		perror("listen");
		exit(3);
	}

	// add listener to the master set
	FD_SET(listener, &master);

	//keep track of biggest fd
	fdmax = listener;

	//main loop
	while (true)
	{
		read_fds = master; // copy the master set
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
		{
			perror("select");
			exit(4);
		}

		// run through existing connections looking for data to read
		for (i = 0; i <= fdmax; i++)
		{
			// if below condition enters then we found one
			if (FD_ISSET(i , &read_fds))
			{
				if (i == listener)
				{
					// handle new connections
					addrlen = sizeof(remoteaddr);
					newfd = accept(listener, reinterpret_cast<struct sockaddr *>(&remoteaddr), &addrlen);
					if (newfd == -1)
						perror("accept");
					else
					{
						send(newfd, hello.c_str(), strlen(hello.c_str()), 0);
						// add to master set
						FD_SET(newfd, &master);
						// keep track of max fd
						if (newfd > fdmax)
							fdmax = newfd;
						std::cout << "webserv: new connection" << std::endl;
					}
				}
				else
				{
					// handle data from a client
					nBytes = recv(i, buff, sizeof(buff), 0);
					if (nBytes <= 0)
					{
						// either error or connection closed by client
						if (nBytes == 0)
							std::cout << "webserv: socket " << i << " hung up" << std::endl;
						else
							perror("recv");
						close(i);
						// remove from master set
						FD_CLR(i, &master);
					}
					else
					{
						// we got some data
						for (j = 0; j <= fdmax; j++)
						{
							// send this to all connections
							if (FD_ISSET(j, &master))
							{
								// except the listener and ourselves
								if (j == i)
								{
									if (send(j, buff, nBytes, 0) == -1)
										perror("send");
								}
							}
						}
					}
				}
			}
		}
		return (0);
	}
}
