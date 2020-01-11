#include "stdafx.h"
#include "stdafx.cpp"
#include "main.h"

#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char *argv[]) {

	WSADATA wsaData;
	struct addrinfo hints , *res, *p;
	int status;
	char ipstr[INET6_ADDRSTRLEN];

	// VERIFICAREA NUMARULUI DE ARGUMENTE
	if (argc != 2){
		fprintf(stderr, "usage: showip hostname\n");
		WSACleanup();
		return 1;
	}

	// WINSOCK STARTUP CHECK
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		fprintf(stderr, "WSAStartup failed: %d\n");
		WSACleanup();
		return 3;
	}

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	// VERIFICAREA HOSTULUI (PRIMUL PARAMETRU) PENTRU CORECTITUDINE 
	if ((status = getaddrinfo("www.google.ro", NULL, &hints, &res)) != 0) {
		fprintf(stderr,"getaddrinfo: %s\n",gai_strerror(status));
		WSACleanup();
		return 2;
	}

	
	printf("IP addresses for %s:\n\n",argv[1]);

	int s = socket(res->ai_family,res->ai_socktype, res-> ai_protocol);

	printf("%d", s);
	while (1);
	for (p = res; p != NULL; p = p->ai_next) {
		void *addr;
		char *ipver;
		
		if (p->ai_family == AF_INET) {
			struct sockaddr_in *ipv4 = (struct sockaddr_in *) p->ai_addr;
			addr = &(ipv4->sin_addr);
			ipver = "IPv4";
		}
		else {
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) p->ai_addr;
			addr = &(ipv6->sin6_addr);
			ipver = "IPv6";
		}
		inet_ntop(p->ai_family,addr,ipstr,sizeof ipstr);
		printf("  %s: %s\n",ipver,ipstr);
	
	}
	freeaddrinfo(res);
	WSACleanup();
	return 0;
}