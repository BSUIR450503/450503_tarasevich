#include "defines.h"

#ifdef __linux__

typedef void (*fun_t)(void*);

void* lDescriptor;

void  (*fileReader) (pthread_mutex_t*);
void* (*fileWriter) (void*);

void connectLibrary() 
{
    lDescriptor = dlopen("./Linux_Shared.so", RTLD_LAZY);

    if (!lDescriptor)
    {
	    puts(dlerror());
	    exit(EXIT_FAILURE);
    }
    fileWriter = dlsym(lDescriptor, "fileWriter");
    fileReader = (fun_t)dlsym(lDescriptor, "fileReader");
}

void createThread()
{
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

    pthread_t threadId;
    if (pthread_create(&threadId, NULL, fileWriter, (void*) &mutex) != 0)
    {
	    fputs("Creating thread error", stderr);
	    exit(EXIT_FAILURE);
    }

    fileReader (&mutex);
    pthread_join(threadId, NULL);
    pthread_mutex_destroy(&mutex);
}

void disconnectLibrary()
{
    dlclose(lDescriptor);
}

#elif _WIN32

HMODULE hDynamicLib;

void (*fileReader) (CRITICAL_SECTION* critical_section);
void (*fileWriter)(LPVOID p_critical_section);
DWORD WINAPI run_write(LPVOID p_critical_section);

void connectLibrary()
{
    hDynamicLib = LoadLibrary("Windows_Shared.dll");

    if (!hDynamicLib)
    {
        fputs("Library loading error", stderr);
        exit(EXIT_FAILURE);
    }

    fileReader  = (void (*)() ) GetProcAddress(hDynamicLib, "fileReader");
}

void createThread()
{
    CRITICAL_SECTION critical_section;
    InitializeCriticalSection(&critical_section);
    
    HANDLE writer = CreateThread(NULL, 0, run_write, &critical_section, 0, NULL);
    if (writer == NULL)
    {
        fputs("Creating thread error", stderr);
        exit(GetLastError());
    }
    
    (*fileReader) (&critical_section);
    WaitForSingleObject(writer, INFINITE);
    DeleteCriticalSection(&critical_section);
}

void disconnectLibrary()
{
    FreeLibrary(hDynamicLib);
}

DWORD WINAPI run_write(LPVOID p_critical_section)
{
    HMODULE h_dyn_lib = LoadLibrary("Windows_Shared.dll");

    if (!h_dyn_lib)
    {
        fputs("Library loading error", stderr);
        exit(EXIT_FAILURE);
    }

    fileWriter = (void (*)() ) GetProcAddress(hDynamicLib, "fileWriter");
    fileWriter(p_critical_section);
    
    FreeLibrary(h_dyn_lib);
    return 0;
}

#endif