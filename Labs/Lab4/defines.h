#pragma once
#ifdef __linux__
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#elif _WIN32

#include <Windows.h>

HANDLE mutex = CreateMutex(NULL, FALSE, NULL);

#endif

const char * threadsStrs[] = { "1. First thread\r\n", "2. Second thread\r\n", 
                               "3. Third thread\r\n", "4. Fourth thread\r\n", 
                               "5. Fifth thread\r\n", "6. Sixth thread\r\n",
                               "7. Seventh thread\r\n", "8. Eighth thread\r\n", 
                               "9. Ninth thread\r\n", "10. Tenth thread\r\n" };