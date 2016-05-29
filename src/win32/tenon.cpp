

#include "stdafx.h"
#include "tenon.h"
//#define _CRT_SECURE_NO_WARNINGS 1
#define _WINSOCK_DEPRECATED_NO_WARNINGS TRUE

typedef struct _PER_HANDLE_DATA
{
	SOCKET Socket;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

typedef struct
{
	WSAOVERLAPPED wsaOverlapped;
	WSABUF wsaBuf;
	int OperationType;
} PER_IO_DATA, *LPPER_IO_DATA;

static DWORD WINAPI ClientWorkerThread(LPVOID lpParameter)
{
	HANDLE hCompletionPort = (HANDLE)lpParameter;
	DWORD bytesCopied = 0;
	OVERLAPPED *overlapped = 0;
	LPPER_HANDLE_DATA PerHandleData;
	LPPER_IO_DATA PerIoData;
	DWORD SendBytes, RecvBytes;
	DWORD Flags;
	wchar_t log_str[128];

	while (TRUE)
	{
		if (GetQueuedCompletionStatus(hCompletionPort, &bytesCopied, (LPDWORD)&PerHandleData, (LPOVERLAPPED*)&PerIoData, INFINITE) == 0)
		{
			swprintf(log_str, 128, L"GetQueuedCompletionStatus filed.\n");
			OutputDebugString(log_str);
		}
		else
		{
			Flags = 0;
			ZeroMemory(&(PerIoData->wsaOverlapped), sizeof(WSAOVERLAPPED));

			PerIoData->wsaBuf.len = 1000;
			WSARecv(PerHandleData->Socket, &(PerIoData->wsaBuf), 1, &RecvBytes, &Flags, &(PerIoData->wsaOverlapped), NULL);

			wchar_t wtext[1024];
			size_t outSize;
			mbstowcs_s(&outSize, wtext, strlen(PerIoData->wsaBuf.buf) + 1, PerIoData->wsaBuf.buf, strlen(PerIoData->wsaBuf.buf) - 1);

			swprintf(log_str, 128, L"receive data: %s\n", wtext);
			OutputDebugString(log_str);
		}
	}

	return 0;
}

DWORD WINAPI init_tenon(HINSTANCE hInstance)
{
	char motise_host[128] = "192.168.13.231";
	unsigned int motise_port = 6424;

	wchar_t log_str[128];

	// Step 1 - Create an I/O completion port.
	HANDLE hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// Step 2 - Find how many processors.
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	// Step 3 - Create worker threads.
	for (int i = 0; i < (int)systemInfo.dwNumberOfProcessors; i++)
	{
		HANDLE hThread = CreateThread(NULL, 0, ClientWorkerThread, hCompletionPort, 0, NULL);
		CloseHandle(hThread);
	}

	// Step 4 - Create a socket.
	SOCKET mortise_conn = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


	unsigned long lNonBlocking = 1;
	if (ioctlsocket(mortise_conn, FIONBIO, &lNonBlocking) != NO_ERROR)
	{
		swprintf(log_str, 128, L"ioctlsocket failed with error: %d\n", GetLastError());
		OutputDebugString(log_str);
		exit(-1);
	}

	PER_HANDLE_DATA *pPerHandleData = new PER_HANDLE_DATA;
	pPerHandleData->Socket = mortise_conn;

	struct hostent *host;
	host = gethostbyname(motise_host);

	SOCKADDR_IN SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
	SockAddr.sin_port = htons(motise_port);

	// Step 5 - Associate the socket with the I/O completion port.
	CreateIoCompletionPort((HANDLE)mortise_conn, hCompletionPort, (DWORD)pPerHandleData, 0);


	WSAEVENT EventArray[WSA_MAXIMUM_WAIT_EVENTS];
	DWORD EventTotal = 0;
	EventArray[EventTotal] = WSACreateEvent();
	if ((WSAEventSelect(mortise_conn, EventArray[EventTotal], FD_CONNECT | FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR))
	{
		swprintf(log_str, 128, L"WSAEventSelect error: %d\n", WSAGetLastError());
		OutputDebugString(log_str);
	}

	EventTotal++;
	int res;
	int index;


	if (WSAConnect(mortise_conn, (SOCKADDR*)(&SockAddr), sizeof(SockAddr), NULL, NULL, NULL, NULL) != 0)
	{
		int err_no = WSAGetLastError();
		if (err_no != WSAEWOULDBLOCK)
		{
			swprintf(log_str, 128, L"WSAConnect error: %d\n", err_no);
			OutputDebugString(log_str);
			exit(-1);
		}
	}

	WSANETWORKEVENTS NetworkEvents;

	while (TRUE)
	{
		//OutputDebugString("loop\n");
		if ((index = WSAWaitForMultipleEvents(EventTotal, EventArray, FALSE, 1, FALSE)) == WSA_WAIT_FAILED)
		{
			swprintf(log_str, 128, L"WSAWaitForMultipleEvents failed with error: %d\n", WSAGetLastError());
			OutputDebugString(log_str);
			continue;
		}
		//OutputDebugString("loop1\n");

		if (index != WSA_WAIT_TIMEOUT)
		{
			res = WSAEnumNetworkEvents(mortise_conn, EventArray[index - WSA_WAIT_EVENT_0], &NetworkEvents);

			if (NetworkEvents.lNetworkEvents & FD_CONNECT)
			{
				swprintf(log_str, 128, L"FD_CONNECT \n");
				OutputDebugString(log_str);
			}

			if (NetworkEvents.lNetworkEvents & FD_READ)
			{

				swprintf(log_str, 128, L"FD_READ \n");
				OutputDebugString(log_str);
			}

			if (NetworkEvents.lNetworkEvents & FD_WRITE)
			{
				swprintf(log_str, 128, L"FD_WRITE \n");
				OutputDebugString(log_str);
			}

			if (NetworkEvents.lNetworkEvents & FD_CLOSE)
			{
				if (NetworkEvents.iErrorCode[FD_CLOSE_BIT] != 0)
				{
					swprintf(log_str, 128, L"FD_CLOSE failed with error: %d\n", NetworkEvents.iErrorCode[FD_CLOSE_BIT]);
					OutputDebugString(log_str);
				}
				else
				{
					swprintf(log_str, 128, L"FD_CLOSE is OK!!! : %d\n ", NetworkEvents.iErrorCode[FD_CLOSE_BIT]);
					OutputDebugString(log_str);
				}
			}
		}
	}


	/*

	static char buffer[1024];
	memset(buffer, 0, 1023);

	PER_IO_DATA *pPerIoData = new PER_IO_DATA;

	pPerIoData->wsaBuf.buf = buffer;
	pPerIoData->wsaBuf.len = sizeof(buffer);

	DWORD dwSendBytes = 0;
	DWORD dwReceivedBytes = 0;
	DWORD dwFlags = 0;

	SecureZeroMemory((PVOID)&pPerIoData->wsaOverlapped, sizeof(pPerIoData->wsaOverlapped));
	pPerIoData->wsaOverlapped.hEvent = WSACreateEvent();

	WSARecv(mortise_conn, &pPerIoData->wsaBuf, 1, &dwReceivedBytes, &dwFlags, &pPerIoData->wsaOverlapped, NULL);
	std::cout << pPerIoData->wsaBuf.buf;*/

	for (;;)
	{
		int nError = WSAGetLastError();
		if (nError != WSAEWOULDBLOCK && nError != 0)
		{
			swprintf(log_str, 128, L"Winsock error code: %d.\nServer disconnected!\n", nError);
			OutputDebugString(log_str);

			//shutdown(mortise_conn, SD_SEND);
			//closesocket(mortise_conn);

			//exit(-1);
		}

		Sleep(1000);
	}

	delete pPerHandleData;
	//delete pPerIoData;
}



int main(int argc, char* argv[])
{
	return 0;
}