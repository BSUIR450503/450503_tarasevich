#pragma once
#ifdef __linux__

#include <pthread.h>

void* threadPrint(void*);

#elif _WIN32

#include <Windows.h>

DWORD threadPrint(void*);
int getch_noblock();
void closeMutex();

#endif

class Thread
{
private:
	#ifdef __linux__
    pthread_t* thread;
    #elif _WIN32
    HANDLE* thread;
    #endif
	int index;
	bool printFlag;
	bool closeFlag;
public:
    Thread(int);
    void startPrint();
    void endPrint();
    bool canPrint();
    bool isWaitingForPrint();
    void closeThread();
    bool isClose();
    int getIndex();
    #ifdef __linux__
    pthread_t getInner();
    #elif _WIN32
    ~Thread();
    #endif
};