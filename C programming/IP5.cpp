// IP5.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

//#include <sys/socket.h>
//#include <netdb.h>
//#include <arpa/inet.h>
//#include <netinet/in.h>

using namespace std;
#include "stdafx.cpp"
#define PORT "80"

#pragma comment(lib, "Ws2_32.lib")

int main() {

	WSADATA wsaData;
	struct addrinfo hints, *res, *p;
	int status;
	char ipstr[INET6_ADDRSTRLEN];
	const char query[] =
		"GET / HTTPS/1.0\r\n"
		"Host: www.9gag.com\r\n"
		"\r\n";
	char hostname[] = "www.9gag.com";

	// VERIFICAREA NUMARULUI DE ARGUMENTE

	//#include <openssl/ssl.h>

	// first connect to the remote as usual, but use the port 443 instead of 80

	// initialize OpenSSL - do this once and stash ssl_ctx in a global var
	//SSL_load_error_strings();
	//SSL_library_init();
	//SSL_CTX *ssl_ctx = SSL_CTX_new(SSLv23_client_method());

	// create an SSL connection and attach it to the socket
	//SSL *conn = SSL_new(ssl_ctx);
	//SSL_set_fd(conn, sock);

	// perform the SSL/TLS handshake with the server - when on the
	// server side, this would use SSL_accept()
	//int err = SSL_connect(conn);
	//if (err != 1)
	//	abort(); // handle error

	// now proceed with HTTP traffic, using SSL_read instead of recv() and
	// SSL_write instead of send(), and SSL_shutdown/SSL_free before close()

	// WINSOCK STARTUP CHECK
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		printf( "WSAStartup failed: %d\n");
		WSACleanup();
		return 3;
	}

	
	printf("Insert an IP Address: \n");
//	scanf_s("%40s",hostname);
	printf("Insert query: \n");
	//scanf("%40s", query);


	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	
	// EXTRAGEM INFORMATIILE DE RETEA DE PE HOST SI PORT 80
	if ((status = getaddrinfo(hostname,PORT , &hints, &res)) != 0) {
		printf("getaddrinfo: %s\n", gai_strerror(status));
		WSACleanup();
		return 2;
	}

	
	//printf("IP addresses for %s:\n\n", argv[1]);

	if (socket(AF_INET,SOCK_DGRAM,0) == -1) {
	
		perror("socket");
		exit(1);
	}

	int s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	/*if (res->ai_family == AF_INET) {
		struct sockaddr_in *ipv4 = (struct sockaddr_in *) res->ai_addr;
		void *addr = &(ipv4->sin_addr);		
	}*/

	if (connect(s, res->ai_addr, res->ai_addrlen) == -1) {
	
		closesocket(s);
		wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
		perror("Can't connect to specified host");
		WSACleanup();
	}

	const char * cp = query;
	int n_written;
	int remaining = strlen(query);
	while (remaining) {
		n_written = send(s, cp, remaining, 0);
		if (n_written <= 0) {
			perror("send");
			return 1;
		}
		remaining -= n_written;
		cp += n_written;
	}



	char buf[1024];
	while (1) {
		int result = recv(s, buf, sizeof(buf), 0);
		if (result == 0) {
			break;
		}
		else if (result < 0) {
			perror("recv");
			closesocket(s);
			return 1;
		}
		fwrite(buf, 1, result, stdout);
	}
	
	closesocket(s);
	freeaddrinfo(res);
	WSACleanup();
	return 0;
}

