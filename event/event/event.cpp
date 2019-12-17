#include <stdio.h>
#include <windows.h>

HANDLE hEvent1, hEvent2,hEvent3;
int a[5];
int b[5];
HANDLE hThr;
HANDLE hThr2;
unsigned long uThrID;

void Thread(void* pParams)
{

	int i, num = 0;

	while (1)
	{
		WaitForSingleObject(hEvent1, INFINITE);
		ResetEvent(hEvent1);
		for (i = 0; i < 5; i++) a[i] = num;
		num++;
	
		SetEvent(hEvent2);
		
	}
}
void Thread1(void* pParams)
{

	int i, num = 0;

	while (1)
	{
		WaitForSingleObject(hEvent2, INFINITE);
		ResetEvent(hEvent2);
		Sleep(rand() % 2000+228);
		SetEvent(hEvent3);

	}
}

int main(void)
{
	hEvent1 = CreateEvent(NULL, TRUE, TRUE, NULL);

	if (hEvent1 == NULL) {
		printf("CreateEvent failed: %d\n", GetLastError());
		return 1;
	}

	hEvent2 = CreateEvent(NULL, TRUE, FALSE, NULL);

	if (hEvent2 == NULL) {
		printf("CreateEvent failed: %d\n", GetLastError());
		return 1;
	}

	hThr = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread, NULL, 0, &uThrID);

	if (hThr == NULL) {
		printf("CreateThread failed: %d\n", GetLastError());
		return 1;
	}

	while (1)
	{
		WaitForSingleObject(hEvent2, INFINITE);
		ResetEvent(hEvent2);
		Sleep(500);
		printf("%d %d %d %d %d\n", a[0], a[1], a[2], a[3], a[4]);
		SetEvent(hEvent1);
	}
	return 0;
}