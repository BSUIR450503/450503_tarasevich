#include "Thread.h"
#include "header.h"

int main(int argc, char *argv[])
{
    #ifdef __linux__
	initscr();
    clear();
    noecho();
    refresh();
    nodelay(stdscr, TRUE);
    #endif
    char symbol;
    int threadsCount = 0;
    int currentThread = 1;
    Thread* thread[THREADS_NUMBER];
    while(TRUE)
    {
    	symbol = getch_noblock();
    	Sleep(SHORT_SLEEP);
        fflush(stdout);
        #ifdef __linux__
        refresh();
        #endif

    	switch (symbol)
    	{
    		case '+':
    		    if (threadsCount < THREADS_NUMBER)
    		    {
    		    	thread[threadsCount++] = new Thread(threadsCount + 1);
    		    }
    		    break;

    		case '-':
    		    if (threadsCount > 0)
    		    {
    		    	thread[--threadsCount]->closeThread();
    		    }
    		    break;

    		case 'q':
    		    if (threadsCount > 0)
    		    {
    		    	for (int i = 0; i < threadsCount; i++)
    		    	{
    		    		thread[i]->closeThread();
    		    		#ifdef __linux__
    		    		pthread_join(thread[i]->getInner(), NULL);
    		    		#endif
    		    	}
    		    }
    		    #ifdef __linux__
    		    clear();
    		    endwin();
    		    #elif _WIN32
    		    closeMutex();
    		    #endif
    		    return 0;
    	}
    	if (threadsCount && thread[currentThread - 1]->canPrint())
        {
            if (currentThread >= threadsCount)
                currentThread = 1;
            else
                currentThread++;
            thread[currentThread - 1]->startPrint();
        }
        #ifdef __linux__
        refresh();
        #endif
    }
}