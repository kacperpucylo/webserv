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

std::map<std::string, std::string> initialize_mime_types(void)
{
    std::map<std::string, std::string> mime_types;

	mime_types[".aac"]      = "audio/aac";
	mime_types[".abw"]      = "application/x-abiword";
	mime_types[".arc"]      = "application/octet-stream";
	mime_types[".avi"]      = "video/x-msvideo";
	mime_types[".azw"]      = "application/vnd.amazon.ebook";
	mime_types[".bin"]      = "application/octet-stream";
	mime_types[".bz"]       = "application/x-bzip";
	mime_types[".bz2"]      = "application/x-bzip2";
	mime_types[".csh"]      = "application/x-csh";
	mime_types[".css"]      = "text/css";
	mime_types[".csv"]      = "text/csv";
	mime_types[".doc"]      = "application/msword";
	mime_types[".epub"]     = "application/epub+zip";
	mime_types[".gif"]      = "image/gif";
	mime_types[".htm"]      = "text/html";
	mime_types[".html"]     = "text/html";
	mime_types[".ico"]      = "image/x-icon";
	mime_types[".ics"]      = "text/calendar";
	mime_types[".jar"]      = "Temporary Redirect";
	mime_types[".jpeg"]     = "image/jpeg";
	mime_types[".jpg"]      = "image/jpeg";
	mime_types[".js"]       = "application/js";
	mime_types[".json"]     = "application/json";
	mime_types[".mid"]      = "audio/midi";
	mime_types[".midi"]     = "audio/midi";
	mime_types[".mpeg"]     = "video/mpeg";
	mime_types[".mpkg"]     = "application/vnd.apple.installer+xml";
	mime_types[".odp"]      = "application/vnd.oasis.opendocument.presentation";
	mime_types[".ods"]      = "application/vnd.oasis.opendocument.spreadsheet";
	mime_types[".odt"]      = "application/vnd.oasis.opendocument.text";
	mime_types[".oga"]      = "audio/ogg";
	mime_types[".ogv"]      = "video/ogg";
	mime_types[".ogx"]      = "application/ogg";
	mime_types[".png"]      = "image/png";
	mime_types[".pdf"]      = "application/pdf";
	mime_types[".ppt"]      = "application/vnd.ms-powerpoint";
	mime_types[".rar"]      = "application/x-rar-compressed";
	mime_types[".rtf"]      = "application/rtf";
	mime_types[".sh"]       = "application/x-sh";
	mime_types[".svg"]      = "image/svg+xml";
	mime_types[".swf"]      = "application/x-shockwave-flash";
	mime_types[".tar"]      = "application/x-tar";
	mime_types[".tif"]      = "image/tiff";
	mime_types[".tiff"]     = "image/tiff";
	mime_types[".ttf"]      = "application/x-font-ttf";
	mime_types[".txt"]      = "text/plain";
	mime_types[".vsd"]      = "application/vnd.visio";
	mime_types[".wav"]      = "audio/x-wav";
	mime_types[".weba"]     = "audio/webm";
	mime_types[".webm"]     = "video/webm";
	mime_types[".webp"]     = "image/webp";
	mime_types[".woff"]     = "application/x-font-woff";
	mime_types[".xhtml"]    = "application/xhtml+xml";
	mime_types[".xls"]      = "application/vnd.ms-excel";
	mime_types[".xml"]      = "application/xml";
	mime_types[".xul"]      = "application/vnd.mozilla.xul+xml";
	mime_types[".zip"]      = "application/zip";
	mime_types[".3gp"]      = "video/3gpp audio/3gpp";
	mime_types[".3g2"]      = "video/3gpp2 audio/3gpp2";
	mime_types[".7z"]		= "application/x-7z-compressed";

	return (mime_types);
}

char *parse(char *line, const char *symbol)
{
    char *message = NULL;
    char *token = strtok(line, symbol);

	token = strtok(NULL, " ");
	if (!token)
		return (NULL);
	message = token;
	return (message);
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

	std::string hello = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 12\r\n\r\nHello world!";

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
					buff[nBytes] = 0;
					std::string request = buff;
					std::map<std::string, std::string> types = initialize_mime_types();
					char *message = parse(buff, " ");
					if (!message)
					{
						// do some shit
					}
					else
					{
						char *copy = strdup(message);
						copy = parse(copy, ".");
						if (copy)
						{
							std::map<std::string, std::string>::iterator it = types.find("." + std::string(copy));
							if (it != types.end())
								std::cout << "Found: " << it->second << std::endl;
						}
						
					}
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
	}
	return (0);
}
