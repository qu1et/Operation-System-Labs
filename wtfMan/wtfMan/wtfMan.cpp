#include <Windows.h>
#include <stdio.h>
            
CRITICAL_SECTION crit;
HANDLE hTr1, hTr2;

int global = 1;

DWORD WINAPI ThreadEven(LPVOID lpParam) {
	while (global < 10) {
		EnterCriticalSection(&crit);
		if (global % 2 == 0) {
			printf("Even - %d\n", global++);
		}
		LeaveCriticalSection(&crit);
	}

	return 0;
}

DWORD WINAPI ThreadOdd(LPVOID lpParam) {
	while (global < 10) {
		EnterCriticalSection(&crit);
		if (global % 2 != 0) {
			printf("Odd - %d\n", global++);
		}
		LeaveCriticalSection(&crit);
	}

	return 0;
}

int main()
{
	InitializeCriticalSection(&crit);

	hTr1 = CreateThread(NULL, 0, ThreadEven, NULL, 0, NULL);

	if (hTr1 == NULL) {
		printf("CreateThread failed: %d\n", GetLastError());
		return 1;
	}

	hTr2 = CreateThread(NULL, 0, ThreadOdd, NULL, 0, NULL);

	if (hTr2 == NULL) {
		printf("CreateThread failed: %d\n", GetLastError());
		return 1;
	}

	WaitForSingleObject(hTr1, INFINITE);
	WaitForSingleObject(hTr2, INFINITE);

	CloseHandle(hTr1);
	CloseHandle(hTr2);
	
	DeleteCriticalSection(&crit);

	return 0;
}