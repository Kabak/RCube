#include "stdafx.h"

#include "RCubeNetwork.h"
#include "assert.h"

RCubeNetwork::RCubeNetwork()
{
// Порт используемый приложением для вывода данных
//	   OUTGOING_PORT = 1235;
// Порт используемый приложением для ввода данных
//	   INCOMING_PORT = 1234;

// Максимальное количество клиентов
		 MAX_CLIENTS = 32;
// Максимальное количество соединений
		 MAX_PEERS = 10;
// Очередь необработанных пакетов		 
		 MAX_PACKETS = 50;
// Очередь необработанных событий		 
		 MAX_EVENTS = 100;

// Set the flags to pass to GetAdaptersAddresses
			   flags = GAA_FLAG_INCLUDE_PREFIX;
// default to unspecified address family (both)
			  family = AF_INET;
//			  family = AF_UNSPEC;
			lpMsgBuf = NULL;

		   outBufLen = sizeof(IP_ADAPTER_ADDRESSES);
		  pAddresses = (IP_ADAPTER_ADDRESSES *)MALLOC(outBufLen);

// Make an initial call to GetAdaptersAddresses to get the 
// size needed into the outBufLen variable
if (GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen) == ERROR_BUFFER_OVERFLOW) 
	{
		FREE(pAddresses);
		pAddresses = (IP_ADAPTER_ADDRESSES *)MALLOC(outBufLen);
	}
			   IPStr = new char[IP_STR_LENGTH];
	    ThisServerIP = new char[IP_STR_LENGTH];
		  ThisHostIP = new char[IP_STR_LENGTH];
			 TempStr = new char[2048];

	   In_DataBuffer = new char[8128];
	   Temp_Incoming = new char[8128];

	  Out_DataBuffer = new char[8128];
	   Temp_Outgoing = new char[8128];

			  server = nullptr;
			  client = nullptr;
			    peer = nullptr;
			  packet = nullptr;

			 Clients.reserve( MAX_CLIENTS );

}


RCubeNetwork::~RCubeNetwork()
{

}


void RCubeNetwork::Shutdown()
{
	ShutDownRcubeServer();
	ShutDownRcubeClient();
	FREE(pAddresses);

	delete[] IPStr;
	delete[] ThisServerIP;
	delete[] ThisHostIP;
	delete[] TempStr;
	delete[] In_DataBuffer;
	delete[] Out_DataBuffer;
	delete[] Temp_Incoming;
	delete[] Temp_Outgoing;

	Clients.clear();
}


bool RCubeNetwork::InitRcubeNetwork(HWND m_hwnd)
{
	if (enet_initialize() != 0)
	{
		MessageBox(m_hwnd, L"An error occurred while initializing ENet", L"Error", MB_OK);
		return false;
	}

		atexit(enet_deinitialize);

	return true;
}


bool RCubeNetwork::InitRcubeServer(HWND m_hwnd)
{
	char * CurrentIP;

	if (GetThisComputerIp() == -1)
	{
		Shutdown();
		MessageBox(m_hwnd, L"Getting Network local IP problem.", L"Error", MB_OK);
		return false;
	}

	CurrentIP = GetCurrentIp();


	/* Bind the server to the default localhost.     */
	/* A specific host address can be specified by   */
	/* enet_address_set_host (& address, "x.x.x.x"); */
	ServerAddress.host = ENET_HOST_ANY;
//	enet_address_set_host(&ServerAddress, CurrentIP);
	/* Bind the server to port 1234. */
	ServerAddress.port = INCOMING_PORT;
	server = enet_host_create(&ServerAddress /* the address to bind the server host to */,
	MAX_CLIENTS/* allow up to 32 clients and/or outgoing connections */,
		2      /* allow up to 2 channels to be used, 0 and 1 */,
		0      /* assume any amount of incoming bandwidth */,
		0      /* assume any amount of outgoing bandwidth */);
	if (server == NULL)
	{
		MessageBox(m_hwnd, L"An error occurred while trying to create an ENet server host.", L"Error", MB_OK);
		Shutdown();
		exit(EXIT_FAILURE);
		return false;
	}

	strcpy_s(ThisServerIP, 20, CurrentIP);

	return true;
}

bool RCubeNetwork::InitRcubeClient(HWND m_hwnd)
{
	char * CurrentIP;

	if (GetThisComputerIp() == -1)
	{
		Shutdown();
		MessageBox(m_hwnd, L"Getting Network local IP problem.", L"Error", MB_OK);
		return false;
	}

	CurrentIP = GetCurrentIp();

	enet_address_set_host(&ClientAddress, CurrentIP);
	ClientAddress.port = OUTGOING_PORT;

	client = enet_host_create(&ClientAddress,//NULL /* the address to bind the server host to */,
		1      /* allow up to 32 clients and/or outgoing connections */,
		2      /* allow up to 2 channels to be used, 0 and 1 */,
		0      /* assume any amount of incoming bandwidth */,
		0      /* assume any amount of outgoing bandwidth */);

	if (client == NULL)
	{
		MessageBox(m_hwnd, L"An error occurred while trying to create an ENet server host.", L"Error", MB_OK);
		Shutdown();
		exit(EXIT_FAILURE);
		return false;
	}

	return true;
}

int RCubeNetwork::CheckRcubeClientEvent()
{
	int Event = 0; // нет событий
	/* Wait up to 1000 milliseconds for an event. */
	while (enet_host_service(client, &event, 0) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			printf("A new client connected from %x:%u.\n",
				event.peer->address.host,
				event.peer->address.port);
			/* Store any relevant client information here. */
			// Сохраняем адрес и порт клиента.
			ClientAddress.host = event.peer->address.host;
			ClientAddress.port = event.peer->address.port;
			Event = 1;

			event.peer->data = "Client information";
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			printf("A packet of length %u containing %s was received from %s on channel %u.\n",
				event.packet->dataLength,
				event.packet->data,
				event.peer->data,
				event.channelID);
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);
			Event = 2;
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			printf("%s disconnected.\n", event.peer->data);
			/* Reset the peer's client information. */
			event.peer->data = NULL;
			Event = 3;
		}
	}
	return Event;
}


int RCubeNetwork::RcubeSendPacket()
{
ENetPacket * packet = enet_packet_create("Привет !",
	strlen("Привет !") + 1,
	ENET_PACKET_FLAG_RELIABLE);

/* Extend the packet so and append the string "foo", so it now */
/* contains "packetfoo\0"*/

//enet_packet_resize(packet, strlen("packetfoo") + 1);
//strcpy(&packet->data[strlen("packet")], "foo");

/* Send the packet to the peer over channel id 0. */
/* One could also broadcast the packet by         */
/* enet_host_broadcast (host, 0, packet);         */
enet_peer_send(peer, 0, packet);

return true;
}

bool RCubeNetwork::AddClient()
{

	return false;
}


int RCubeNetwork::CheckRcubeServerEvent()
{
	int Event = 0; // нет событий
	/* Wait up to 1000 milliseconds for an event. */
	while (enet_host_service(server, &event, 0) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
/*			printf("A new client connected from %x:%u.\n",
				event.peer->address.host,
				event.peer->address.port);
			/* Store any relevant client information here. */
//			event.peer->data = "Client information";
// Сохраняем адрес и порт клиента.
			ClientAddress.host = event.peer->address.host;
			ClientAddress.port = event.peer->address.port;
			Event = 1;

			break;
		case ENET_EVENT_TYPE_RECEIVE:
			printf("A packet of length %u containing %s was received from %s on channel %u.\n",
				event.packet->dataLength,
				event.packet->data,
				event.peer->data,
				event.channelID);
			/* Clean up the packet now that we're done using it. */
			enet_packet_destroy(event.packet);
			Event = 2;
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			printf("%s disconnected.\n", event.peer->data);
			/* Reset the peer's client information. */
			event.peer->data = NULL;
			Event = 3;
		}
	}
	return Event;
}


bool RCubeNetwork::ConnectToServer()
{
	/* Connect to some.server.net:1234. */
	enet_address_set_host(&ServerAddress, "10.0.0.68"); // 192.168.1.103  10.0.0.68 "some.server.net"
	ServerAddress.port = 1234;
	/* Initiate the connection, allocating the two channels 0 and 1. */
	peer = enet_host_connect(client, &ServerAddress, 2, 0);
	if (peer == NULL)
	{
		fprintf(stderr,
			"No available peers for initiating an ENet connection.\n");
		exit(EXIT_FAILURE);
	}
	/* Wait up to 5 seconds for the connection attempt to succeed. */
	if (enet_host_service(client, &event, 10) > 0 && //5000
		event.type == ENET_EVENT_TYPE_CONNECT)
	{
		puts("Connection to some.server.net:1234 succeeded.");

	}
	else
	{
		/* Either the 5 seconds are up or a disconnect event was */
		/* received. Reset the peer in the event the 5 seconds   */
		/* had run out without any significant event.            */
//		enet_peer_reset(peer);
//		puts("Connection to some.server.net:1234 failed.");
	}
	return true;
}


bool RCubeNetwork::DisconnectFromServer()
{
enet_peer_disconnect(peer, 0);
/* Allow up to 3 seconds for the disconnect to succeed
* and drop any packets received packets.
*/
while (enet_host_service(client, &event, 10) > 0)   //  C 10 сервер получает сообщение об отключении
{
	switch (event.type)
	{
	case ENET_EVENT_TYPE_RECEIVE:
		enet_packet_destroy(event.packet);
		break;
	case ENET_EVENT_TYPE_DISCONNECT:
		puts("Disconnection succeeded.");

	}
}
/* We've arrived here, so the disconnect attempt didn't */
/* succeed yet.  Force the connection down.             */
enet_peer_reset(peer);

return true;
}


void RCubeNetwork::ShutDownRcubeServer()
{
	if ( server != nullptr )	
		enet_host_destroy(server);
	server = nullptr;
}


void RCubeNetwork::ShutDownRcubeClient()
{
	if (server != nullptr)
		enet_host_destroy(client);
	client = nullptr;
}


int RCubeNetwork::GetThisComputerIp()
{

	IP_ADAPTER_ADDRESSES* ActiveAdapter;

	DWORD dwRetVal = GetAdaptersAddresses(family, flags, NULL, pAddresses, &outBufLen);

	ActiveAdapter = pAddresses;
	do
	{
		if (ActiveAdapter->OperStatus != 1 )
			if ( ActiveAdapter->Next != NULL )
				ActiveAdapter = ActiveAdapter->Next;
			else
			{
				return -2; // Нет активного сетевого адаптера в системе
			}
		else
			break;
	}
	while( ActiveAdapter != NULL );

	if (dwRetVal == NO_ERROR) 
	{

		char *Temp = new char[10];

		//	char First = (UINT)pAddresses->FirstUnicastAddress->Address.lpSockaddr->sa_data[2];
		int First = 0;
		int TempByte = 0;

		IPStr[0] = NULL;

		for (int i = 2; i < 6; ++i)
		{
			TempByte = (int)ActiveAdapter->FirstUnicastAddress->Address.lpSockaddr->sa_data[i];

			if (TempByte < 0)
				First = 256 + TempByte;
			else
				First = TempByte;

			sprintf_s(Temp, 10, "%d", First);
			strcat_s(IPStr, IP_STR_LENGTH, Temp);
			strcat_s(IPStr, IP_STR_LENGTH, ".");
		}

		TempByte = (int)strlen(IPStr);

		if (TempByte > 0)
			IPStr[TempByte - 1] = NULL;

		delete[] Temp;
		return 1; // IP получен
	}
	else
		return -1; // Ошибка получения IP -1

}


char* RCubeNetwork::GetCurrentIp()
{
	return IPStr;
}

