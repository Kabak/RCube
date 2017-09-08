#pragma once
#include "stdafx.h"

#pragma once
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#include "NetworkClass.h"
#include <vector>
#include "DirectXMath.h"

using namespace std;
using namespace DirectX;

// Link with Iphlpapi.lib
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Ws2_32.lib")

// ���� ������������ ����������� ��� ������ ������
#define OUTGOING_PORT 1235
// ���� ������������ ����������� ��� ����� ������
#define INCOMING_PORT 1234
// ������������ ������ ������ ( �������� ��� ��������� clients )
#define MAX_STR_LENGTH 32
// ������������ ������ ������ IP ������
#define IP_STR_LENGTH 16

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

// ++++++++++++++++++++++++++++++++++++++++++    ��� �������    +++++++++++++++++++++++++++++++++++++++++++++++
class RCubeNetwork
{
public:
	RCubeNetwork();
	~RCubeNetwork();

	// ���� ������������ ����������� ��� ������ ������
//	static UINT16 OUTGOING_PORT;
	// ���� ������������ ����������� ��� ����� ������
//	static UINT16 INCOMING_PORT;

	// ������������ ���������� ���������� ��������
	int MAX_CLIENTS;
	int MAX_PEERS;
	int MAX_PACKETS;
	int MAX_EVENTS;

	// IP detection 
	ULONG outBufLen;
	ULONG flags;
	ULONG family;
	LPVOID lpMsgBuf;

	PIP_ADAPTER_ADDRESSES pAddresses;

	bool InitRcubeNetwork(HWND m_hwnd);
	bool InitRcubeServer(HWND m_hwnd);
	bool InitRcubeClient(HWND m_hwnd);
// �������� �� ������� � ����
	bool CheckRcubeNetworkEvent();
	int CheckRcubeClientEvent();
	int CheckRcubeServerEvent();
// ���������� � ��������
	bool ConnectToServer();
	bool DisconnectFromServer();

	int RcubeSendPacket();

	// �������� ������� � ������ ������������
	bool AddClient();
	// ������� ������� �� ������ ������������
	void DeleteClient();
	// �������� ��������� �� ������ � �������
	bool CheckClient();
	// ������ ������� � ������
	bool StartServer();
	// ��������� ���������� ������
	bool StopServer();
	// ������ ������� � ������
	bool StartClient();
	// ��������� ������ ������� 
	bool StopClient();

	void ShutDownRcubeServer();
	void ShutDownRcubeClient();
	void Shutdown();
// �������� IP ���������� �� �������� ����� ������� ����� � Internet
// � ������ ������ ������������ 1.   � ������ ������ ��������� IP -1
// ���� � ���������� ��� ��������� �������� �������� ������������ -2
	int GetThisComputerIp();

// ������������ IP � ����� ������
	char* GetCurrentIp();

// ������������� ������ IP � UINT
	UINT IPtoUINT (char* IP);
// ������������� UINT � ������ IP 
	void UINToIP (UINT IP, char* IPStr);

	ENetAddress ServerAddress;
	ENetAddress ClientAddress;

// ���������� ��������� ������ �� ���� ��������
	struct _ClientDATA
	{
		XMFLOAT3	Position;
		XMFLOAT3	Rotation;
			 int	Gun;
	};

// ����� ���������
	struct _Message
	{
		char* Text;
		int Rieciver; // ����� ���������� ���������.  ���� -1, �� �������� ���. ���� -2, �� �������� �������
	};


private:
	ENetHost*	server;
	ENetHost*	client;
	ENetPeer*	peer;
	ENetPacket* packet;
	ENetEvent	event;

	// ��������� �������� ������� ��������������� � �������
	struct �lientData
	{
		char ClientName [MAX_STR_LENGTH];
		char Country [MAX_STR_LENGTH];
		UINT IP;
		unsigned short Port;
		int PingRate;
	};

	vector < ENetHost >		Clients;
	vector < �lientData >	ClientsData;

	char* In_DataBuffer;
	char* Out_DataBuffer;
	char* Temp_Incoming;
	char* Temp_Outgoing;

	char* IPStr;
// IP ���������� �� ������� ������ ������
	char* ThisServerIP;
// IP ���������� �� ������� ������ HOST
	char* ThisHostIP;

	char* TempStr;
};



// ++++++++++++++++++++++++++++++++++++++++++    ��� �������    +++++++++++++++++++++++++++++++++++++++++++++++
