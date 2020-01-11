// Protocol1.cpp 
#include "stdafx.h"
#include <iostream>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib,"ws2_32.lib")

// ASCII
// protocol parser
// BINPACK
// BRO
// SPICY
// libtins
// libcrafter
// hammer
// SCAPY

#define MAXCON 4
#define ever (;;)

void new_connection(int sock) {
	int r;
	char error_code;
	int error_code_size = sizeof(error_code);

	while (getsockopt(sock, SOL_SOCKET, SO_ERROR, &error_code, &error_code_size)) {
		r = send(sock, ".\n", 2, 0);
		if (r < 0) break;
		Sleep(1);
	}
	closesocket(sock);
}

int main()
{
	WSAData wsa;
	SOCKADDR_IN address;
	int err, client_sock = 0;
	const char one = 1;
	//memset(address.sin_zero, 0, sizeof address.sin_zero);

	memset((char *)&address, '\0', sizeof(address));

	address.sin_port = htons(1001);
	address.sin_addr.S_un.S_addr = INADDR_ANY;
	address.sin_family = AF_INET;

	err = WSAStartup(MAKEWORD(2, 2), &wsa);
	if (err != 0 ){	
		wprintf(L"WSAStartup failed with error %d\n", err);
		return -1;
	}

	int listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listener == INVALID_SOCKET) {
		wprintf(L"FAILED TO CREATE SOCKET\n");
		WSACleanup();
		return -1;
	}

	err = setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
	if (err == SOCKET_ERROR) {
		wprintf(L"FAILED TO REUSE ADDRESS SETSOCKOPT%d\n", WSAGetLastError());
	}

	err = bind(listener, (struct sockaddr*) &address, sizeof address);
	if (err == SOCKET_ERROR) {
		wprintf(L"FAILED TO BIND SOCKET%d\n", WSAGetLastError());
		WSACleanup();
		return -1;
	}

	err = listen(listener, MAXCON);
	if (err == SOCKET_ERROR) {
		wprintf(L"FAILED TO LISTEN FOR CLIENTS\n");
		WSACleanup();
		return -1;
	}

	for ever{
		client_sock = accept(listener, 0, 0);
		if (client_sock == SOCKET_ERROR) {
			closesocket(client_sock);
			wprintf(L"the client could not be accepted || error: %d\n", WSAGetLastError());
			continue;
		}
		std::thread t(new_connection, client_sock);
		t.detach();

	}

	err = closesocket(listener);
	if (err == SOCKET_ERROR) {
		wprintf(L"closesocket function failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	WSACleanup();
    return 0;
}

