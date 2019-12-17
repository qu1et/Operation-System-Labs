#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <mutex>
#include <iostream>

using namespace std;

HANDLE hmutex;

void _tmain(int argc, TCHAR* argv[])
{
	STARTUPINFOW process_startup_info{ 0 };
	process_startup_info.cb = sizeof(process_startup_info);

	PROCESS_INFORMATION process_info{ 0 };

	wchar_t commandline_args[] = L"C:\\Users\\alkad\\OneDrive\\Рабочий стол\\Operation-System-Labs\\mutex\\test\\Debug\\test.exe";

	hmutex = CreateMutex(NULL, FALSE, L"Global\\myMute");

	if (hmutex == NULL) {
		cout << "CreateMutex failed: " << GetLastError() << endl;
	}

	cout << "Mutex has been created: " << hmutex << endl;
		
	if (CreateProcessW(NULL, commandline_args, NULL, NULL, TRUE, 0, NULL, NULL, &process_startup_info, &process_info))
	{
		WaitForSingleObject(process_info.hProcess, INFINITE);
		CloseHandle(process_info.hProcess);
		CloseHandle(process_info.hThread);
	}

	wchar_t command_args[] = L"C:\\Users\\alkad\\OneDrive\\Рабочий стол\\Operation-System-Labs\\mutex\\test2\\Debug\\test2.exe";

	if (CreateProcessW(NULL, command_args, NULL, NULL, TRUE, 0, NULL, NULL, &process_startup_info, &process_info))
	{
		WaitForSingleObject(process_info.hProcess, INFINITE);
		CloseHandle(process_info.hProcess);
		CloseHandle(process_info.hThread);
	}
	
	CloseHandle(hmutex);
}
