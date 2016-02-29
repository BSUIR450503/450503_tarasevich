#include <iostream>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32

#include <Windows.h>
#include <conio.h>

#define PID HANDLE

#elif __linux__

#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#define PID pid_t

#endif

using namespace std;

class ProcessFamily
{
public:
	void parent(PID&, int, char**);
	void child(PID&, int, char**);
};

void ProcessFamily::parent(PID &myPid, int argc, char* argv[])
{
#ifdef _WIN32

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	// there we should enter the name of existing executable file
	TCHAR CommandLine[] = TEXT("Proc_child");

	// pipe's name
	LPTSTR PipeName = TEXT("\\\\.\\pipe\\MyPipe");
	int buff;
	DWORD BytesToWrite, BytesToRead;

	BytesToRead = sizeof(int);
	BytesToWrite = sizeof(int);

	// ZeroMemory fills the memory with nulls
	ZeroMemory(&si, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	ZeroMemory(&pi, sizeof(pi));

	// creating named pipe
	myPid = CreateNamedPipe(
		PipeName,					// имя канала
		PIPE_ACCESS_DUPLEX,			// чтение и запись из канала
		PIPE_TYPE_BYTE |			// передача по байтам
		PIPE_READMODE_BYTE |		// режим чтения по байтам
		PIPE_WAIT,					// синхронная передача
		1,							// число экземпляров канала
		1024 * 8,					// размер выходного буфера
		1024 * 8,					// размер входного буфера
		NMPWAIT_USE_DEFAULT_WAIT,	// тайм-аут клиента
		NULL);						// защита по умолчанию

	if (myPid == INVALID_HANDLE_VALUE)
	{
		cout << "CreatePipe failed. Error code: " << (int)GetLastError() << endl;
		exit(-1);
	}

	// creating a process
	if (!CreateProcess(NULL,	// Нет имени модуля (используется командная строка).
		CommandLine,			// Командная строка.
		NULL,					// Дескриптор процесса не наследуемый.
		NULL,					// Дескриптор потока не наследуемый.
		FALSE,					// Установим наследование дескриптора в FALSE.
		0,						// Флажков создания нет.
		NULL,					// Используйте блок конфигурации родителя.
		NULL,					// Используйте стартовый каталог родителя.
		&si,					// Указатель на структуру STARTUPINFO.
		&pi)					// Указатель на структуру PROCESS_INFORMATION.
		)
	{
		cout << "CreateProcess failed. Error code: " << (int)GetLastError() << endl;
		exit(-2);
	}

	// checking connection
	if (!ConnectNamedPipe(myPid, NULL))
	{
		cout << "Client couldn't connect." << endl;
		exit(-3);
	}

	cout << "Enter number to process: ";
	cin >> buff;

	WriteFile(myPid, &buff, sizeof(int), &BytesToWrite, NULL);

	WaitForSingleObject(pi.hProcess, INFINITE);

	ReadFile(myPid, &buff, sizeof(int), &BytesToRead, NULL);

	cout << "Action's result is " << buff << endl;

	CloseHandle(myPid);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	_getch();

#endif


#ifdef __linux__

	int fd[2];		// file descriptor
	int num;		// number to act
	int result;		// result
	char fd0[8], fd1[8];


	// Creating a pipe
	if (pipe(fd) == -1)
	{
		perror("pipe() failed");
		exit(1);
	}
	else
	{
		cout << "pipe() successful!" << endl;
	}


	// Creating a new process and making an action with num
	myPid = fork();

	if (myPid < 0)
	{
		perror("fork failed");
		exit(2);
	}
	else if (myPid == 0)
	{
		/* Child process */

		// convert descriptors to char* to be able to send them into child
		// and use as well
		snprintf(fd0, sizeof(fd0), "%d", fd[0]);
		snprintf(fd1, sizeof(fd1), "%d", fd[1]);

		// exec from current working catalog
		execl("Proc_Child", fd0, fd1, NULL);
		exit(0);
	}
	else if (myPid > 0)
	{
		/* Parent process */

		cout << "Enter number to process: ";
		cin >> num;

		// moving num to pipe
		write(fd[1], &num, sizeof(int));

		close(fd[1]);

		wait(NULL);

		read(fd[0], &result, sizeof(int));
		
		close(fd[0]);
		

		cout << "The result is " << result << endl;
	}

#endif
}

void ProcessFamily::child(PID &myPid, int argc, char* argv[])
{
#ifdef _WIN32

	// pipe's name
	LPTSTR PipeName = TEXT("\\\\.\\pipe\\MyPipe");
	int buff;
	DWORD BytesToWrite, BytesToRead;

	BytesToRead = sizeof(int);
	BytesToWrite = sizeof(int);

	// opening existing pipe
	myPid = CreateFile(
		PipeName,	    // имя канала
		GENERIC_READ |  // чтение и запись в канал
		GENERIC_WRITE,
		0,              // нет разделяемых операций 
		NULL,           // защита по умолчанию
		OPEN_EXISTING,  // открытие существующего канала 
		0,              // атрибуты по умолчанию
		NULL);          // нет дополнительных атрибутов 

	ReadFile(myPid, &buff, sizeof(int), &BytesToRead, NULL);

	buff *= buff;

	WriteFile(myPid, &buff, sizeof(int), &BytesToWrite, NULL);

	// sleep for 5 sec
	cout << "Child process will sleep for 5 seconds." << endl;
	Sleep(5000);

	CloseHandle(myPid);

#endif


#ifdef __linux__

	int fd[2];
	int resNum;

	fd[0] = atoi(argv[0]);
	fd[1] = atoi(argv[1]);

	// reading "num" from pipe
	read(fd[0], &resNum, sizeof(int));

	close(fd[0]);

	// resNum^2
	resNum *= resNum;

	// writing "num" to pipe
	write(fd[1], &resNum, sizeof(int));

	close(fd[1]);

	// sleep for 5 sec
	cout << "Child process will sleep for 5 seconds." << endl;
	usleep(5000000);

#endif
}