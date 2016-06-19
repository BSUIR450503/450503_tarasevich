#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32

#include <windows.h>
#define TWENTY_MB 20971520

#elif __linux__

#include <unistd.h>
#include <dlfcn.h>
#include <pthread.h>
#include <errno.h>

#define TWENTY_MB 2097

#endif

void connectLibrary();
void createThread();
void disconnectLibrary();