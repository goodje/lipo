//created by goodje


#include "launcher.h"

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
	BOOL bRet;

	while (TRUE)
	{
		bRet = GetQueuedCompletionStatus(hCompletionPort, &bytesCopied, (LPDWORD)&PerHandleData, (LPOVERLAPPED*)&PerIoData, INFINITE);

		if (bytesCopied == 0)
		{
			break;
		}
		else
		{
			Flags = 0;
			ZeroMemory(&(PerIoData->wsaOverlapped), sizeof(WSAOVERLAPPED));

			PerIoData->wsaBuf.len = 1000;
			WSARecv(PerHandleData->Socket, &(PerIoData->wsaBuf), 1, &RecvBytes, &Flags, &(PerIoData->wsaOverlapped), NULL);
		}
	}

	return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char log_str[128];

	WSADATA WsaDat;
	WSAStartup(MAKEWORD(2, 2), &WsaDat);

	// Step 1 - Create an I/O completion port.
	HANDLE hCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	// Step 2 - Find how many processors.
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);

	//OutputDebugString(systemInfo.dwNumberOfProcessors);

	// Step 3 - Create worker threads.
	for (int i = 0; i < (int)systemInfo.dwNumberOfProcessors; i++)
	{
		HANDLE hThread = CreateThread(NULL, 0, ClientWorkerThread, hCompletionPort, 0, NULL);
		CloseHandle(hThread);
	}

	// Step 4 - Create a socket.
	SOCKET Socket = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	PER_HANDLE_DATA *pPerHandleData = new PER_HANDLE_DATA;
	pPerHandleData->Socket = Socket;

	struct hostent *host;
	host = gethostbyname("localhost");

	SOCKADDR_IN SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
	SockAddr.sin_port = htons(8888);

	// Step 5 - Associate the socket with the I/O completion port.
	CreateIoCompletionPort((HANDLE)Socket, hCompletionPort, (DWORD)pPerHandleData, 0);

	WSAConnect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr), NULL, NULL, NULL, NULL);

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

	WSARecv(Socket, &pPerIoData->wsaBuf, 1, &dwReceivedBytes, &dwFlags, &pPerIoData->wsaOverlapped, NULL);
	std::cout << pPerIoData->wsaBuf.buf;

	for (;;)
	{
		int nError = WSAGetLastError();
		if (nError != WSAEWOULDBLOCK && nError != 0)
		{
			sprintf(log_str, "Winsock error code: %d.\nServer disconnected!\n", nError);
			OutputDebugString(log_str);
			
			shutdown(Socket, SD_SEND);
			closesocket(Socket);

			exit(-1);
		}

		Sleep(1000);
	}

	delete pPerHandleData;
	delete pPerIoData;
	WSACleanup();

	return 0;
}



