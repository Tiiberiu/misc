// ARP_packet_broadcast.cpp : Defines the entry point for the console application.

// SEND ARP REQUESTS TO CHECK WHO IS ACTIVE ON THE LOCAL NETWORK

#include "stdafx.h"


#include <iostream>
#include <string>

#include <WinSock2.h>
#include <WS2tcpip.h>

#include <iphlpapi.h>
#include <assert.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")

// define the arp packet structure
// create a raw socket on which to send the packet
// <opt> make the socket non-blocking so process faster
// packet will be send to broadcast address
//

#define	IP_MAXPACKET 65535
#define arp_header_lenght 28
#define ipv4_header_lenght 20
#define ethernet_header_lenght 14
#define max_adapters 20

struct ARP_PACKET {

	WORD hardware_type;		// 2  bytes
	WORD protocol_type;		// 2  bytes
	BYTE hardware_size;		// 1  byte
	BYTE protocol_size;		// 1  byte
	WORD op_code;			// 2  bytes
	BYTE sender_mac[6];		// 6  bytes
	BYTE sender_ip[4];		// 4  bytes
	BYTE target_mac[6];		// 6  bytes
	BYTE target_ip[4];		// 4  bytes
							// 28 bytes
} arp_packet;


//Look for an interface with OperStatus == 1 && AdapterIp != 0.0.0.0 && AdapterGateway != 0.0.0.0
struct IPv4AdapterStr {

	BYTE MacAddress[MAX_ADAPTER_ADDRESS_LENGTH];
	BYTE AdapterDescription[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
	char* AdapterName;
	BYTE AdapterIp[16];
	char AdapterGateway[16];
	ULONG AdapterType;
	
	IF_OPER_STATUS OperStatus;

} detected_adapters[max_adapters];



static void PrintMACaddress(unsigned char MACData[])
{
	printf("%02X-%02X-%02X-%02X-%02X-%02X",
		MACData[0], MACData[1], MACData[2], MACData[3], MACData[4], MACData[5]);
}


int main()
{
	// ADDR STRUCTURES
	int one = 1;
	int adapter_cnt = 0, ipinfo_cnt;

	BYTE ethernet_frame[IP_MAXPACKET];

	addrinfo hints, *results;
	sockaddr_in *IPv4;

	IP_ADAPTER_ADDRESSES pAddresses[max_adapters];
	IP_ADAPTER_INFO pInfo[max_adapters];

	DWORD pInfo_size = sizeof(pInfo);
	DWORD pAddresses_size = sizeof(pAddresses);
	

	// WINSOCK STARTUP
	WSAData wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		std::cerr << "WSAStartup failed with error " << WSAGetLastError() << '\n';
		return EXIT_FAILURE;
	}

	// CREATE THE SOCKET

	

	//WINDOWS SPECIFIC WAY TO GET MAC

	int adapter_err = GetAdaptersAddresses(AF_INET, 0, NULL, pAddresses, &pAddresses_size);
	assert(adapter_err == ERROR_SUCCESS);
		PIP_ADAPTER_ADDRESSES pAdapterInfo = pAddresses;           // Contains pointer to current adapter info

		do {

			memcpy(detected_adapters[adapter_cnt].MacAddress ,pAdapterInfo->PhysicalAddress, sizeof(pAdapterInfo->PhysicalAddress));
			detected_adapters[adapter_cnt].AdapterName = pAdapterInfo->AdapterName;
			detected_adapters[adapter_cnt].AdapterType = pAdapterInfo->IfType;
			detected_adapters[adapter_cnt++].OperStatus = pAdapterInfo->OperStatus;

#pragma region printAdapter
			PrintMACaddress(pAdapterInfo->PhysicalAddress);                 // Print MAC address
			std::cout << "  " << pAdapterInfo->AdapterName;
			switch (pAdapterInfo->IfType) {
			case MIB_IF_TYPE_ETHERNET:
				std::cout << "  " << "ETHERNET";
				break;
			case MIB_IF_TYPE_LOOPBACK:
				std::cout << "  " << "LOOPBACK";
				break;

			case IF_TYPE_IEEE80211:
				std::cout << "  " << "WI-FI";
				break;

			default:
				std::cout << "  " << pAdapterInfo->IfType;
			}
			std::cout << "  " << pAdapterInfo->OperStatus << '\n';
#pragma endregion
			pAdapterInfo = pAdapterInfo->Next;                    

		} while (pAdapterInfo);
	

	int info_err = GetAdaptersInfo(pInfo, &pInfo_size);
	assert(info_err == ERROR_SUCCESS);
	PIP_ADAPTER_INFO pAdapterIps = pInfo;           // Contains pointer to current adapter info

	do {
		for (ipinfo_cnt = 0; ipinfo_cnt < adapter_cnt; ipinfo_cnt++) {
			if (!strcmp(pAdapterIps->AdapterName ,detected_adapters[ipinfo_cnt].AdapterName)) {
				memcpy(detected_adapters[ipinfo_cnt].AdapterGateway, pAdapterIps->GatewayList.IpAddress.String, sizeof(pAdapterIps->GatewayList.IpAddress.String));
				memcpy(detected_adapters[ipinfo_cnt].AdapterIp, pAdapterIps->IpAddressList.IpAddress.String, sizeof(pAdapterIps->IpAddressList.IpAddress.String));
				memcpy(detected_adapters[ipinfo_cnt].AdapterDescription, pAdapterIps->Description, sizeof(pAdapterIps->Description));
			}

		}
		/*std::cout << pAdapterIps->Description << "   ";
		std::cout << pAdapterIps->GatewayList.IpAddress.String<<'\n';*/
		pAdapterIps = pAdapterIps->Next;

	} while (pAdapterIps);
	int i;
	for (i = 0; i < adapter_cnt; i++) {
		if (detected_adapters[i].OperStatus == 1 && strcmp(detected_adapters[i].AdapterGateway, "0.0.0.0")) {
			std::cout << detected_adapters[i].AdapterDescription << '\n';
			std::cout << detected_adapters[i].AdapterIp << '\n';
			std::cout << detected_adapters[i].AdapterGateway << '\n';
			break;
		}

	}

	//We got: ADAPTER MAC, LOCAL IP ADDRESS and DEFAULT GATEWAY (destination ip address)
	// DEFAULT GATEWAY MAC = ff ff ff ff ff ff

	int err_inet = inet_pton(AF_INET, (char*)detected_adapters[i].AdapterIp, &arp_packet.sender_ip);
	if (err_inet != 1) {
		std::cerr << "inet_pton1 failed with error " << WSAGetLastError() << '\n';
		return EXIT_FAILURE;
	}

	int err_inet2 = inet_pton(AF_INET, (char*)detected_adapters[i].AdapterGateway, &arp_packet.target_ip);
	if (err_inet2 != 1) {
		std::cerr << "inet_pton2 failed with error " << WSAGetLastError() << '\n';
		return EXIT_FAILURE;
	}

	memset(arp_packet.target_mac, 0xff, 6 * sizeof(BYTE));
	memset(arp_packet.sender_mac, 0, 6 * sizeof(BYTE));

	memcpy(arp_packet.sender_mac, detected_adapters[i].MacAddress, sizeof(arp_packet.sender_mac));

	arp_packet.hardware_type = htons(1); //ETHERNET
	arp_packet.protocol_type = htons(0x0800); //ARP TYPE

	arp_packet.hardware_size = 6;        // MAC ADDRESS SIZE
	arp_packet.protocol_size = 4; // IP ADDRESS SIZE

	arp_packet.op_code = htons(1); // REQUEST CODE

	/*printf("Leading text " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(arp_packet.sender_mac[0] & 0xFF));
	printf("Leading text " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(arp_packet.sender_mac[1] & 0xFF));
	printf("Leading text " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(arp_packet.sender_mac[2] & 0xFF));
	printf("Leading text " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(arp_packet.sender_mac[3] & 0xFF));
	printf("Leading text " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(arp_packet.sender_mac[4] & 0xFF));
	printf("Leading text " BYTE_TO_BINARY_PATTERN "\n", BYTE_TO_BINARY(arp_packet.sender_mac[5] & 0xFF));*/

	int current_frame_lenght = ethernet_header_lenght + arp_header_lenght;

	memcpy(ethernet_frame, arp_packet.target_mac, 6 * sizeof (BYTE) ); //0-5
	memcpy(ethernet_frame + 6, arp_packet.sender_mac, 6 * sizeof(BYTE));//6-11

	ethernet_frame[12] = 8;
	ethernet_frame[13] = 6;

	memcpy(ethernet_frame + ethernet_header_lenght, &arp_packet, arp_header_lenght * sizeof(BYTE));

	//admin priority required
	int arp_sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
	if (arp_sock == INVALID_SOCKET) {
		std::cerr << "socket()  failed with error " << WSAGetLastError() << '\n';
		return EXIT_FAILURE;
	}

	if (setsockopt(arp_sock, IPPROTO_IP, IP_HDRINCL, (char *)&one, sizeof(one)) == SOCKET_ERROR)
	{
		std::cerr << "setsockopt()  failed with error " << WSAGetLastError() << '\n';
		return EXIT_FAILURE;
	}
	int bytes;
	if ((bytes = sendto(arp_sock, (char *)ethernet_frame, current_frame_lenght, 0, (struct sockaddr *) &IPv4, sizeof(IPv4))) <= 0) {
		perror("sendto() failed");
		exit(EXIT_FAILURE);
	}


	// IMPOSSIBLE TO CONTINUE WITHOUT layer2 driver



    return 0;
}

