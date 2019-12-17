#include <iostream>
#include <Windows.h>
#include <process.h>
#include <queue>
#include <time.h>

//Описание функции потока
void ThreadRun(void* params);
//Функция которая будет выводить массив в консоль
void ArrayShow(int arr[]);
//Функция будет заполнять массив случайными числами
void ArrayChange(int arr[]);
//Наш объект(массив), к которому должен быть синхронизирован доступ
int myArray[10];
//Критическая секция для массива
CRITICAL_SECTION csArray;

int main(int argc, char* argv[])
{
	//Заполним массив числами
	for (int i = 0; i < 10; ++i)
		myArray[i] = i;

	//Инициализируем критическую секцию
	InitializeCriticalSection(&csArray);

	std::cout << "Array before:\n";
	ArrayShow(myArray);
	//Запустим 5 потоков
	for (int i = 0; i < 5; ++i)
	{
		//Каждому потоку, в качестве параметра передаем его номер
		int* number = new int;
		*number = i;
		_beginthread(ThreadRun, 0, (void*)number);
	}

	//Ждем, что бы пользователь сообщил о завершении потоков
	char isFinished = 'n';
	while (isFinished != 'y')
	{
		std::cout << "MAIN_THREAD: All threads is finished? (y/n)\n";
		std::cin >> isFinished;
	}

	//Удаляем критическую секцию
	DeleteCriticalSection(&csArray);

	std::cout << "Array after:\n";
	ArrayShow(myArray);

	//Пауза
	system("PAUSE");

	return 0;
}

void ThreadRun(void* params)
{
	//Преобразуем входящие параметры в int*
	int* myNumber = (int*)params;
	std::string start;
	std::string enter;
	std::string leave;

	char buff[10];
	//itoa(*myNumber, buff, 10);
	sprintf_s(buff, "%d", *myNumber);

	start.append("Thread["); start.append(buff); start.append("] start...\n");
	enter.append("Thread["); enter.append(buff); enter.append("] enter in critical section...\n");
	leave.append("Thread["); leave.append(buff); leave.append("] leave critical section...\n");

	std::cout << start.c_str();

	//Входим в критическую секцию
	EnterCriticalSection(&csArray);
	std::cout << enter.c_str();
	//Задержка для наглядности процесса
	Sleep(1000);
	ArrayChange(myArray);
	ArrayShow(myArray);
	//Выходим из критической секции
	std::cout << leave.c_str();
	LeaveCriticalSection(&csArray);
	
}

void ArrayShow(int arr[])
{
	std::string line;
	for (int i = 0; i < 10; ++i)
	{
		char buff[10];
		//itoa(arr[i], buff, 10);
		sprintf_s(buff, "%d", arr[i]);
		line.append(buff);
		line.append("-");
	}
	line.append("\n");
	std::cout << line.c_str();
}

void ArrayChange(int arr[])
{
	srand(time(NULL));
	for (int i = 0; i < 10; ++i)
	{
		myArray[i] = rand() % 100;
	}
}