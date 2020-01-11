// Froxy.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <time.h>
#include <thread>
#include <stdio.h>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include "fork.h"

#pragma comment(lib, "WS2_32.lib")

#define PORT "21" // the port client will be connecting to 
#define FTP_PORT 21
#define FTP_ADDR "90.130.70.73"
#define MAXDATASIZE 512 // max number of bytes we can get at once 
#define conn 5
#define FTP_PASV_CODE 227
#define CONNECT_TIMEOUT	30
#define MIN_PORT	1024
#define MAX_LINE	500
#define MAX_LOGLINE	300
#define NTOP_BUFS	3
#define TCP_BACKLOG	10
#define ever (;;)
//#define max(X,Y) ((X) > (Y) ? (X) : (Y))
enum { CMD_NONE = 0, CMD_PORT, CMD_PASV };

int proxy_IP[4];

struct session {

	UINT		id;
	int				receivingclient_sock;
	int				destinationServer_sock;

	struct sockaddr_in  receivingclient_INFO;
	struct sockaddr_in  destinationServer_INFO;

	char			 recvbuf[MAX_LINE];
	size_t			 recvbuf_valid;

	char			 sendbuf[MAX_LINE];
	size_t			 sendbuf_valid;

	int				status;
} client[conn + 1];


int RecvAll(SOCKET client_socket, void *data, int data_size)
{
	char *data_ptr = (char*)data;
	int bytes_recv;

	while (data_size > 0)
	{
		bytes_recv = recv(client_socket, data_ptr, data_size, 0);
		if (bytes_recv <= 0)
			return bytes_recv;

		data_ptr += bytes_recv;
		data_size -= bytes_recv;
	}

	return 1;
}


void initNetworkDriver() {

	WSADATA wsaData;
	int err = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (err != 0) {
		std::cerr << "WSAStartup failed with error: " << err << "\r\n";
		exit(EXIT_FAILURE);
	}

}

void rate_control(clock_t t, int rate, int size, double magic) {
	long desire_time, t_delay;

	printf("buffer size = %d bytes\n", size);
	desire_time = magic*((long)(size*1000000.0) / (rate*1024.0));    // 10^(-6) sec
	printf("desire time = %ld\n", desire_time);
	printf("t = %ld usec\n", t);

	if (t < desire_time) {   // you can change the number to control rate!
		t_delay = (int)(desire_time - t);
		printf("delay = %ld\n", t_delay);
		Sleep(t_delay / 1000);
	}
}

int createServer(int port) {

	int listensock, reuse = 1, reuselen = sizeof(int);
	char adrs[INET_ADDRSTRLEN];
	struct sockaddr_in server_info;
	struct addrinfo hints, *p;
	
	memset(&hints, 0, sizeof hints);

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	listensock = socket(AF_INET, SOCK_STREAM, 0);
	if (listensock == INVALID_SOCKET) {

		std::cerr << "Socket creation failed with err: " << WSAGetLastError() << "\r\n";
		exit(EXIT_FAILURE);

	}

	int gai_err = getaddrinfo(NULL, std::to_string(port).c_str(), &hints, &p);
	if(gai_err != 0){

		std::cerr << "getaddrinfo failed with err: " << WSAGetLastError() << "\r\n";
		exit(EXIT_FAILURE);
	}
	
	inet_ntop(AF_INET, &((struct sockaddr_in *) p->ai_addr)->sin_addr, adrs, INET_ADDRSTRLEN);
	//std::cout << " Got a new connection from " << adrs << " !\n";

	if (setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse, reuselen) == SOCKET_ERROR) {

		std::cerr << "setsockopt _reuse_ failed with err: " << WSAGetLastError() << "\r\n";
		exit(EXIT_FAILURE);
	}

	if (bind(listensock, p->ai_addr , p->ai_addrlen) < 0) {

		std::cerr << "Bind on socket failed with err: " << WSAGetLastError() << "\r\n";
		exit(EXIT_FAILURE);
	}

	freeaddrinfo(p);

	if (listen(listensock, conn) < 0) {

		std::cerr << "Listen on socket failed with err: " << WSAGetLastError() << "\r\n";
		exit(EXIT_FAILURE);
	}


	return listensock;
}

// connect to server and proxy the banner back
int connect_FTP(int clifd, int ser_port = 21, char addr[] = FTP_ADDR ) {

	int sockfd;
	int byte_num;
	char buffer[MAXDATASIZE];
	struct sockaddr_in servaddr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("[x] Create socket error");
		return EXIT_FAILURE;
	}

	memset(&servaddr, 0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(ser_port);

	if (inet_pton(AF_INET, addr, &servaddr.sin_addr) <= 0) {
		printf("[v] Inet_pton error for %s", addr);
		return EXIT_FAILURE;
	}

	if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		printf("[x] Connect error");
		return EXIT_FAILURE;
	}

	printf("[v] Connect to FTP server\n");
	if (ser_port == FTP_PORT) {
		if ((byte_num = recv(sockfd, buffer, MAXDATASIZE, 0)) <= 0) {
			printf("[x] Connection establish failed.\n");
		}

		if (send(clifd, buffer, byte_num, 0) < 0) {
			printf("[x] Write to client failed.\n");
			return EXIT_FAILURE;
		}
	}

	return sockfd;
}


// user:pass:ip:port
void replaceCwithNULL( char * string, char c ){

	int i = 0;
	while (string[i++] != '\0')
		if (string[i] == c) {
			string[i] = '\0';
			break;
		}

}

int authentificate_session(int client_sock, char * ip_address, char * port) {

	int i = 0;
	char username[21];
	char password[21];
	// 000.000.000.000
	// 65555
	int result;
		
	if ((result = RecvAll(client_sock, &username[0], 20)) == 1) {
			//check user
		username[20] = '\0';
		replaceCwithNULL(username, '|');

		if (strcmp(username, "admin") != 0)
			return 1;

		if ((result = RecvAll(client_sock, &password[0], 20)) == 1) {
			// check pass
			password[20] = '\0';
			replaceCwithNULL(password, '|');
			if (strcmp(password, "root") != 0)
				return 1;

			if ((result = RecvAll(client_sock, &ip_address[0], 16)) == 1) {
				// validate ip , remove trailing 0zs
				
				ip_address[15] = '\0';
				replaceCwithNULL(ip_address, '|');

				if (strcmp(ip_address, "192.168.001.001") != 0)
					return 1;

				if ((result = RecvAll(client_sock, &port[0], 5)) == 1) {
					//validate port and return session ok
					port[5] = '\0';
					replaceCwithNULL(port, '|');

					if (strcmp(port, "21") != 0)
						return 1;

					return EXIT_SUCCESS;
				}
			}				
		}			
	}


	return 1;
}

int startProxySession(int ser_port, int user_socket, int rate, bool download) {
	// SESSION ENVIRONMENT
	char buffer[MAXDATASIZE];
	int distanceServer_sock = -1, data_sock = -1, connfd;
	int data_port;
	int byte_num;
	int status, pasv[7];
	int childpid;
	int size, old_size, new_size;
	socklen_t userlen;
	struct sockaddr_in cliaddr;

	int maxfdp1;
	int i, nready = 0;
	fd_set rset, session_set;

	clock_t t1 = 0, t2 = 0, t;     // uplaod start time

	unsigned long int buf_size = sizeof(buffer);
	printf("size of buffer = %lu\n", buf_size);

	//GET SERVER IP AND PORT


	// connect to FTP server
	if ((distanceServer_sock = connect_FTP(user_socket, ser_port)) < 0) {
		printf("[x] Connect to FTP server failed.\n");
		return EXIT_FAILURE;
	}

	data_sock = distanceServer_sock;

	// initialize select vars
	FD_ZERO(&session_set);
	FD_SET(user_socket, &session_set);
	FD_SET(distanceServer_sock, &session_set);

	for ever {
		// reset select vars
		rset = session_set;
		maxfdp1 = max(user_socket, distanceServer_sock) + 1;

		// select descriptor
		nready = select(maxfdp1 + 1, &rset, NULL, NULL, NULL);
		if (nready > 0) {
			// check FTP client socket fd
			if (FD_ISSET(user_socket, &rset)) {
				memset(buffer, '\0', MAXDATASIZE);
				if ((byte_num = recv(user_socket, buffer, MAXDATASIZE,0)) <= 0) {
					printf("[!] Client terminated the connection.\n");
					break;
				}
				if (size == 0) {
					size = strlen(buffer) * sizeof(char);
					old_size = size;
				}
				else
					old_size = size;
				size = strlen(buffer) * sizeof(char);
				t2 = t1;
				t1 = clock();
				printf("t1 = %ld, t2 = %ld", t1, t2);
				t = t1 - t2;

				rate_control(t, rate, old_size, 2);

				if (send(distanceServer_sock, buffer, byte_num, 0) < 0) {
					printf("[x] Write to server failed.\n");
					break;
				}
			}
			// check FTP server socket fd
			if (FD_ISSET(distanceServer_sock, &rset)) {
				memset(buffer, '\0', MAXDATASIZE);
				if ((byte_num = recv(distanceServer_sock, buffer, MAXDATASIZE, 0)) <= 0) {
					printf("[!] Server terminated the connection.\n");
					break;
				}

				if (ser_port == FTP_PORT)
					buffer[byte_num] = '\0';

				status = atoi(buffer);
				//printf("LINE 173 : buffer %s\n",buffer);
				if (status == FTP_PASV_CODE && ser_port == FTP_PORT) {

					sscanf_s(buffer, "%d Entering Passive Mode (%d,%d,%d,%d,%d,%d)", &pasv[0], &pasv[1], &pasv[2], &pasv[3], &pasv[4], &pasv[5], &pasv[6]);
					memset(buffer, '\0', MAXDATASIZE);
					//sprintf(buffer, "%d Entering Passive Mode (%d,%d,%d,%d,%d,%d)\n", status, proxy_IP[0], proxy_IP[1], proxy_IP[2], proxy_IP[3], pasv[5], pasv[6]);
					// James
					//printf("LINE 180 : buffer %s\n",buffer);
					if ((childpid = fork()) == 0) {
						data_port = pasv[5] * 256 + pasv[6];
						data_sock = createServer(data_port);
						printf("[-] Waiting for data connection!\n");
						userlen = sizeof(struct sockaddr_in);
						connfd = accept(data_sock, (struct sockaddr *)&cliaddr, &userlen);
						if (connfd < 0) {
							printf("[x] Accept failed\n");
							return 0;
						}

						printf("[v] Data connection from: %s:%d connect.\n", cliaddr.sin_addr, htons(cliaddr.sin_port));
						startProxySession(data_port, connfd, rate, true);
						printf("[!] End of data connection!\n");
						exit(0);
					}
				}

				// Download condition (download = true)
				if (download) {
					old_size = size;
					size = strlen(buffer) * sizeof(char);
					t2 = t1;
					t1 = clock();
					printf("t1 = %ld, t2 = %ld\n", t1, t2);
					t = t1 - t2;
					rate_control(t, rate, old_size, 2);
				}

				if (send(user_socket, buffer, byte_num, 0) < 0) {
					printf("[x] Write to client failed.\n");
					break;
				}

			}
		}
		else {
			printf("[x] Select() returns -1. ERROR!\n");
			return -1;
		}
	}

	return 0;

}

int main(int argc, char *argv[])
{
	int sockfd, numbytes, processid, i = 0;
	char buf[MAXDATASIZE];
	char stringaddr[INET_ADDRSTRLEN];
	char remote_ip[17];
	char remote_port[6];
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_in newcon_info;
	SOCKET newcon_sock;
	socklen_t netcon_len;
	int rv;
	char s[INET6_ADDRSTRLEN];

	if (argc < 3) {
		std::cerr << "usage: ./executable [server_ip] [server_port]\n";
		exit(EXIT_FAILURE);
	}

	sscanf_s(argv[1], " %d.%d.%d.%d", &proxy_IP[0], &proxy_IP[1], &proxy_IP[2], &proxy_IP[3]);
	int dummy = atoi(argv[2]);

	initNetworkDriver();

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;


	//Use config file to initialize this structures
	SOCKET ssock = createServer(44444);

	for ever {
		netcon_len = sizeof newcon_info;
		newcon_sock = accept(ssock, (struct sockaddr*)&newcon_info, &netcon_len);
		if (newcon_sock < 0) {
			std::cerr << "Accepting new connection on socket failed with err: " << WSAGetLastError() << "\r\n";
			exit(EXIT_FAILURE);
		}

		inet_ntop(AF_INET, &(newcon_info.sin_addr), stringaddr, INET_ADDRSTRLEN);
		std::cout << "Got a new connection from " << stringaddr << " !\n";

		

		if (authentificate_session(newcon_sock, remote_ip, remote_port) == EXIT_SUCCESS) {

			memcpy(&client[i].receivingclient_INFO, &newcon_info, sizeof(struct sockaddr_in));
			client[i].destinationServer_sock = newcon_sock;

			int gai_err = getaddrinfo("90.130.70.73", remote_port, &hints, &p);
			if (gai_err != 0) {

				std::cerr << "remote server for client" << newcon_sock  << " failed with err: " << WSAGetLastError() << "\r\n";
				exit(EXIT_FAILURE);
			}
			memcpy(&client[i].destinationServer_INFO, (struct sockaddr_in *)p->ai_addr, sizeof(struct sockaddr_in));

			freeaddrinfo(p);

			startProxySession(21, newcon_sock, 0, 0);
		}
		
		closesocket(ssock);
		//startProxySession(FTP_PORT, newcon_sock, 0 , 0);
		exit(EXIT_SUCCESS);
		

		closesocket(newcon_sock);
	}

	//memset(&hints, 0, sizeof hints);
	//hints.ai_family = AF_UNSPEC;
	//hints.ai_socktype = SOCK_STREAM;

	//if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
	//	fprintf(stderr, "getaddrinfo: %d\n", WSAGetLastError());
	//	return 1;
	//}

	//// loop through all the results and connect to the first we can
	//for (p = servinfo; p != NULL; p = p->ai_next) {
	//	if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
	//		perror("client: socket");
	//		continue;
	//	}

	//	if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
	//		perror("client: connect");
	//		closesocket(sockfd);
	//		continue;
	//	}

	//	break;
	//}

	//if (p == NULL) {
	//	fprintf(stderr, "client: failed to connect\n");
	//	return 2;
	//}

	//inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
	//	s, sizeof s);
	//printf("client: connecting to %s\n", s);

	//freeaddrinfo(servinfo); // all done with this structure

	//char buff[1000];
	//int len;
	//char c1 = '\r';
	//char c2 = '\n';

	//while (1) {

	//	if ((numbytes = recv(sockfd, buf, MAXDATASIZE - 1, 0)) == -1) {
	//		std::cerr << " RECV failed with error" << WSAGetLastError();
	//		exit(1);
	//	}

	//	buf[numbytes] = '\0';

	//	printf("%s", buf);

	//	std::cin.getline(buff,sizeof buff);

	//	len = strlen(buff);
	//	buff[len] = c1;
	//	buff[len + 1] = c2;
	//	buff[len + 2] = '\0';

	//	if (send(sockfd, buff, strlen(buff), 0) == -1) {
	//		std::cerr << " SEND failed with error" << WSAGetLastError();
	//		exit(1);
	//	}

	//}
	//closesocket(sockfd);

	return 0;
}
