// ICMPCraft.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h> 
#include <ws2tcpip.h> 
#include <mstcpip.h>
#include <windows.h>
#include <stdint.h>
#include <vector>
#include <algorithm>

#pragma comment (lib, "Ws2_32.lib")


void send_icmp_packet(SOCKET icmp_sock);

struct IPV4_HDR
{
	unsigned char ip_header_len : 4;
	unsigned char ip_version : 4;
	unsigned char ip_tos;
	unsigned short ip_total_length;
	unsigned short ip_id;

	unsigned char ip_frag_offset : 5;

	unsigned char ip_more_fragment : 1;
	unsigned char ip_dont_fragment : 1;
	unsigned char ip_reserved_zero : 1;

	unsigned char ip_frag_offset1;

	unsigned char ip_ttl;
	unsigned char ip_protocol;
	unsigned short ip_checksum;
	unsigned int ip_srcaddr;
	unsigned int ip_destaddr;
};

struct ICMP_HDR
{
	BYTE type;
	BYTE code;
	USHORT checksum;
	USHORT id;
	USHORT seq;
};

unsigned short compute_checksum(unsigned short* buffer, int size)
{
	unsigned long cksum = 0;
	while (size > 1)
	{
		cksum += *buffer++;
		size -= sizeof(unsigned short);
	}
	if (size)
	{
		cksum += *(char*)buffer;
	}

	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	return (unsigned short)(~cksum);
}

void send_receive_ping(SOCKET icmp_sock)
{
	std::vector<char> receive_buffer;
	receive_buffer.resize(65536);
	std::fill(receive_buffer.begin(), receive_buffer.end(), 0);
	char *Buffer = receive_buffer.data();

	int recv_bytes = 0;
	DWORD start_time = GetTickCount();
	bool first_time_in_loop = true;
	do
	{
		if ((first_time_in_loop == true
			|| GetTickCount() - start_time > 5000))
		{
			OutputDebugString(L"Sending an ICMP packet....\n");
			send_icmp_packet(icmp_sock);

			first_time_in_loop = false;
			start_time = GetTickCount();
		}

		recv_bytes = recvfrom(icmp_sock, Buffer, 65536, 0, 0, 0);
		if (recv_bytes > 0)
		{
			// Handle received packet
		}
		else
		{
			break;
		}
	} while (recv_bytes > 0);
}

void send_icmp_packet(SOCKET icmp_sock)
{
	sockaddr_in sockaddr_in_dst = {};
	sockaddr_in_dst.sin_family = AF_INET;
	sockaddr_in_dst.sin_port = 0;
	sockaddr_in_dst.sin_addr.s_addr = inet_addr("192.168.1.1"); // google.com

	std::vector<char> send_buffer;
	send_buffer.resize(sizeof(IPV4_HDR) + sizeof(ICMP_HDR));
	std::fill(send_buffer.begin(), send_buffer.end(), 0);

	IPV4_HDR *ipv4_header = (IPV4_HDR *)send_buffer.data();
	ipv4_header->ip_header_len = 5;
	ipv4_header->ip_version = 4;
	ipv4_header->ip_tos = 16;
	ipv4_header->ip_total_length = htons(send_buffer.size());
	ipv4_header->ip_id = htons(0);
	ipv4_header->ip_ttl = 64;
	//ipv4_header->ip_ttl = 2;
	ipv4_header->ip_protocol = IPPROTO_ICMP;
	ipv4_header->ip_srcaddr = 0;
	ipv4_header->ip_destaddr = sockaddr_in_dst.sin_addr.s_addr;
	ipv4_header->ip_checksum = compute_checksum((unsigned short *)ipv4_header, sizeof(IPV4_HDR));

	static unsigned short seq = 0;
	ICMP_HDR *icmp_header = (ICMP_HDR *)(send_buffer.data() + sizeof(IPV4_HDR));
	icmp_header->type = 8;
	icmp_header->seq = seq++;
	icmp_header->id = 888;
	icmp_header->checksum = compute_checksum((unsigned short *)icmp_header, sizeof(ICMP_HDR));

	int ret = sendto(icmp_sock, (char *)send_buffer.data(), send_buffer.size(),
		0, (sockaddr *)&sockaddr_in_dst, sizeof(sockaddr_in_dst));
}


int main()
{
	WSADATA ws;
	WSAStartup(MAKEWORD(2, 2), &ws);

	SOCKET icmp_sock = socket(AF_INET, SOCK_RAW, IPPROTO_IP);

	char hostname[256];
	gethostname(hostname, sizeof(hostname));

	hostent *local = gethostbyname(hostname);

	sockaddr_in source;
	memset(&source, 0, sizeof(source));
	memcpy(&source.sin_addr.s_addr, local->h_addr_list[0], sizeof(source.sin_addr.s_addr));
	source.sin_family = AF_INET;
	source.sin_port = 0;

	bind(icmp_sock, (sockaddr *)&source, sizeof(source));

	int recv_all_opt = 1;
	int ioctl_read = 0;
	WSAIoctl(icmp_sock, SIO_RCVALL, &recv_all_opt, sizeof(recv_all_opt), 0, 0, (LPDWORD)&ioctl_read, 0, 0);

	int ip_header_include = 1;
	setsockopt(icmp_sock, IPPROTO_IP, IP_HDRINCL, (char *)&ip_header_include, sizeof(ip_header_include));

	send_receive_ping(icmp_sock);

	closesocket(icmp_sock);
	WSACleanup();

	return 0;
}