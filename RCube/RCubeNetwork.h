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

// Порт используемый приложением для вывода данных
#define OUTGOING_PORT 1235
// Порт используемый приложением для ввода данных
#define INCOMING_PORT 1234
// Максимальная длинна строки ( например для структуры clients )
#define MAX_STR_LENGTH 32
// Максимальная длинна строки IP адреса
#define IP_STR_LENGTH 16

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

// ++++++++++++++++++++++++++++++++++++++++++    Мои функции    +++++++++++++++++++++++++++++++++++++++++++++++
class RCubeNetwork
{
public:
	RCubeNetwork();
	~RCubeNetwork();

	// Порт используемый приложением для вывода данных
//	static UINT16 OUTGOING_PORT;
	// Порт используемый приложением для ввода данных
//	static UINT16 INCOMING_PORT;

	// максимальное количество допустимых клиентов
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
// Проверка на события в сети
	bool CheckRcubeNetworkEvent();
	int CheckRcubeClientEvent();
	int CheckRcubeServerEvent();
// Соединение с сервером
	bool ConnectToServer();
	bool DisconnectFromServer();

	int RcubeSendPacket();

	// Дабавить клиента в список подключённых
	bool AddClient();
	// Удалить клиента из списка подключённых
	void DeleteClient();
	// Проверка подключён ли клиент к серверу
	bool CheckClient();
	// Запуск сервера в потоке
	bool StartServer();
	// Остановка серверного потока
	bool StopServer();
	// Запуск клиента в потоке
	bool StartClient();
	// Остановка потока клиента 
	bool StopClient();

	void ShutDownRcubeServer();
	void ShutDownRcubeClient();
	void Shutdown();
// Получить IP компьютера по которому можно реально выйти в Internet
// В случае успеха возвращается 1.   В случае ошибки получения IP -1
// Если в компьютере нет активного сетевого адаптера возвращается -2
	int GetThisComputerIp();

// Возвращается IP в ввиде строки
	char* GetCurrentIp();

// Преобразовать строку IP в UINT
	UINT IPtoUINT (char* IP);
// Преобразовать UINT в строку IP 
	void UINToIP (UINT IP, char* IPStr);

	ENetAddress ServerAddress;
	ENetAddress ClientAddress;

// Глобальная структура данных по всем клиентам
	struct _ClientDATA
	{
		XMFLOAT3	Position;
		XMFLOAT3	Rotation;
			 int	Gun;
	};

// Пакет сообщения
	struct _Message
	{
		char* Text;
		int Rieciver; // Номер получателя сообщения.  Если -1, то получают все. Если -2, то получает команда
	};


private:
	ENetHost*	server;
	ENetHost*	client;
	ENetPeer*	peer;
	ENetPacket* packet;
	ENetEvent	event;

	// Структура описания клиента подключившегося к серверу
	struct СlientData
	{
		char ClientName [MAX_STR_LENGTH];
		char Country [MAX_STR_LENGTH];
		UINT IP;
		unsigned short Port;
		int PingRate;
	};

	vector < ENetHost >		Clients;
	vector < СlientData >	ClientsData;

	char* In_DataBuffer;
	char* Out_DataBuffer;
	char* Temp_Incoming;
	char* Temp_Outgoing;

	char* IPStr;
// IP компьютера на котором создаём сервер
	char* ThisServerIP;
// IP компьютера на котором создаём HOST
	char* ThisHostIP;

	char* TempStr;
};



// ++++++++++++++++++++++++++++++++++++++++++    Мои функции    +++++++++++++++++++++++++++++++++++++++++++++++
