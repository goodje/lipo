//created by goodje


#include "launcher.h"

HANDLE g_hChildStd_IN_Rd = NULL;
HANDLE g_hChildStd_IN_Wr = NULL;
HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	char log_str[128];

	WSADATA WsaDat;
	WSAStartup(MAKEWORD(2, 2), &WsaDat);

	TCHAR szCmdline[] = TEXT("child");
	PROCESS_INFORMATION piProcInfo;
	STARTUPINFO siStartInfo;

	ZeroMemory(&siStartInfo, sizeof(siStartInfo));
	siStartInfo.cb = sizeof(siStartInfo);
	siStartInfo.hStdError = g_hChildStd_OUT_Wr;
	siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
	siStartInfo.hStdInput = g_hChildStd_IN_Rd;
	siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

	ZeroMemory(&piProcInfo, sizeof(piProcInfo));

	if (!CreateProcess(
		"tenon.exe",           // No module name (use command line)
		"",      // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&siStartInfo,   // Pointer to STARTUPINFO structure
		&piProcInfo))   // Pointer to PROCESS_INFORMATION structure
	{
		sprintf(log_str, "CreateProcess failed: %d.\n", GetLastError());
		OutputDebugString(log_str);
		return -1;
	}

	// Wait until child process exits.
	WaitForSingleObject(piProcInfo.hProcess, INFINITE);
	// Close process and thread handles. 
	CloseHandle(piProcInfo.hProcess);
	CloseHandle(piProcInfo.hThread);

	CreateGUI(hInstance, SW_SHOW);



	//init_tenon(hInstance);

	WSACleanup();

	return 0;
}



