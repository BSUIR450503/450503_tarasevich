#include "header.h"
#include "Thread.h"
#include "defines.h"

#ifdef __linux__
void* threadPrint(void* currentThread)
#elif _WIN32
DWORD threadPrint(void* currentThread)
#endif
{
    Thread* thread = (Thread*)currentThread;

    while (TRUE)
    {
    	#ifdef __linux__
    	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    	#endif
    	if (!thread->isWaitingForPrint())
    	{
    		#ifdef __linux__
    		pthread_mutex_lock(&mutex);
    		#elif _WIN32
    		WaitForSingleObject(mutex, INFINITE);
    		#endif
    		int threadNum = thread->getIndex() - 1;

    		for (int i = 0; i < strlen(threadsStrs[threadNum]); i++)
    		{
    			cout << threadsStrs[threadNum][i];
    			fflush(stdout);
    			Sleep(OUT_SLEEP);
    			#ifdef __linux__
    			refresh();
    			#endif
    		}

    		thread->endPrint();
    	}
    	else
    	{
    		Sleep(SHORT_SLEEP);
    		fflush(stdout);
    	#ifdef __linux__
    		refresh();
    	}
    	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    	#elif _WIN32
    	}	
    	#endif

    	if (thread->isClose())
    		break;
    }
    return 0;
}

Thread::Thread(int number)
{
	#ifdef __linux__
	this->thread = new pthread_t();
	#elif _WIN32
	this->thread = new HANDLE();
	#endif
	this->index = number;

	this->printFlag = false;
	this->closeFlag = false;

	#ifdef __linux__
	pthread_create(thread, NULL, threadPrint, (void*)this);
	#elif _WIN32
	CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)threadPrint, (LPVOID)this, NULL, NULL);
	#endif
}

#ifdef _WIN32
Thread::~Thread()
{
	WaitForSingleObject(this->thread, INFINITE);
	CloseHandle(*this->thread);
	delete this->thread;
}
#endif

void Thread::startPrint()
{
	this->printFlag = true;
}

void Thread::endPrint()
{
	this->printFlag = false;
	#ifdef __linux__
	pthread_mutex_unlock(&mutex);
	#elif _WIN32
	ReleaseMutex(mutex);
	#endif
}

bool Thread::canPrint()
{
	#ifdef __linux__
	if (pthread_mutex_trylock(&mutex) != 0)
        return false;

    pthread_mutex_unlock(&mutex);
    #elif _WIN32
    if (WaitForSingleObject(mutex, INFINITE) == WAIT_TIMEOUT)
		return false;
	ReleaseMutex(mutex);
	#endif
    return true;
}

bool Thread::isWaitingForPrint()
{
	return this->printFlag ? false : true;
}

void Thread::closeThread()
{
	this->closeFlag = true;
}

bool Thread::isClose()
{
	return this->closeFlag;
}

int Thread::getIndex()
{
	return this->index;
}
#ifdef __linux__
pthread_t Thread::getInner()
{
	return *this->thread;
}
#elif _WIN32
int getch_noblock()
{
	if (_kbhit())
		return _getch();
	else
		return -1;
}

void closeMutex()
{
	CloseHandle(mutex);
}
#endif