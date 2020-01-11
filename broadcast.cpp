
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define SERVERPORT 7412
#define NRofPORTS 65535


void scanPort_noBLOCK(struct sockaddr_in sa,int i) {

	printf("Scanning port %i\n", i);

	sa.sin_port = htons(i);
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock < 0) {
		exit(1);
	}

	int err = connect(sock, (struct sockaddr*)&sa, sizeof sa);

	if (err < 0) {
		fflush(stdout);
	}
	else {
		printf("Port %i is open\n", i);
	}
	closesocket(sock);

}


void scanPort_BLOCK(struct sockaddr_in sa, int i) {
	long  arg;
	u_long iMode = 1;
	printf("Scanning port %i\n", i);

	sa.sin_port = htons(i);
	int sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock < 0) {
		exit(1);
	}

	if ((arg = ioctlsocket(sock, FIONBIO, &iMode)) != NO_ERROR) {
		
		
		int err = connect(sock, (struct sockaddr *)&sa,sizeof sa);
		if (err == -1)
		{
			if (errno != EINPROGRESS)
			{
				// connect failed, do something...
				return;
			}

			fd_set wfd, efd;

			FD_ZERO(&wfd);
			FD_SET(sock, &wfd);

			FD_ZERO(&efd);
			FD_SET(sock, &efd);

			timeval tv;
				err = select(sock + 1, NULL, &wfd, &efd, &tv);
			if (err == -1)
			{
				// select failed, do something...
				closesocket(sock);
				return;
			}

			if (err == 0)
			{
				// connect timed out...
				closesocket(sock);
				return;
			}

			if (FD_ISSET(sock, &efd))
			{
				// connect failed, do something...
				char so_error[1024];
				int len = 1024;
				getsockopt(sock, SOL_SOCKET, SO_ERROR, so_error,&len);
				closesocket(sock);
				return;
			}
		}
		else {
			printf("Port %i is open\n", i);
		}
		
		closesocket(sock);
	}
	else {
		fprintf(stderr, "Error fcntl(..., F_GETFL) (%s)\n", strerror(errno));
	
	}

}

void ComputeChecksum(unsigned char *data, int len)
{
	long sum = 0;  /* assume 32 bit long, 16 bit short */
	unsigned short *temp = (unsigned short *)data;

	while (len > 1) {
		sum += *temp++;
		if (sum & 0x80000000)   /* if high order bit set, fold */
			sum = (sum & 0xFFFF) + (sum >> 16);
		len -= 2;
	}

	if (len)       /* take care of left over byte */
		sum += (unsigned short) *((unsigned char *)temp);

	while (sum >> 16)
		sum = (sum & 0xFFFF) + (sum >> 16);

	//return ~sum;
}

void main(int argc, char *argv[] ) {

	//variabile socket si optiuni
	int sockfd;
	WSADATA wsadata;
	struct sockaddr_in scanned;
	struct hostent * he;
	int numbytes;
	char  broadcast = '1';
	argc = 3;
	char x[33]; memset(x, 'A', sizeof x - 1); x[sizeof x - 1] = '\0';
	argv[1] = "192.168.0.204";
	argv[2] = x;

	//WSA STARTUP 
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != NULL)
	{
		printf("WSAStartup failed: %d\n");
		WSACleanup();
		exit(1);
	}

	//verificare argumente
	if (argc != 3) {
		fprintf(stderr, "folosinta: app address message\n\r");
		exit(1);
	}

	// verificam hostul pentru existenta
	if ((he = gethostbyname(argv[1])) == NULL) {
		perror("hostname");
		exit(1);
	}
	//CREATE Socket si verificam erori
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}
	//SETSOCKOPT -- making broadcast packets to send
	if (setsockopt(sockfd,SOL_SOCKET,SO_BROADCAST,&broadcast,sizeof broadcast) == -1) {
		perror("setsockopt (SO_BROADCAST)");
		exit(1);
	}
	//INIT THEIR ADDRESS
		scanned.sin_addr = *((struct in_addr *) he->h_addr_list[0]);
		scanned.sin_family = AF_INET;
		scanned.sin_port = htons(SERVERPORT);
	memset(scanned.sin_zero ,'\0',sizeof scanned.sin_zero); 
	int i = 0;
	//SENDTO
	
	if ((numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0, (struct sockaddr *)&scanned, sizeof scanned)) == -1) {
		perror("sendto");
		exit(1);
	}

	//AFIS MESAJ 
	printf("%d) Am trimis %d bytes catre %s\n", ++i,numbytes, inet_ntoa(scanned.sin_addr));

	for (i = 1; i < NRofPORTS; i++) 
		scanPort_BLOCK(scanned, i);
	while (1);



	closesocket(sockfd);
}