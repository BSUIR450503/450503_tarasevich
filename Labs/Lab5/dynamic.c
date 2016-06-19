#include <stdio.h>
#include <stdlib.h>

#ifdef __linux__

#define _XOPEN_SOURCE 600
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <aio.h>
#include <errno.h>
#include <pthread.h>

#define Sleep usleep
#define SHORT_SLEEP 100000
#define LONG_SLEEP 500000

#elif _WIN32

#include <windows.h>
#define SHORT_SLEEP 500
#define LONG_SLEEP 1000

#endif

#define TWENTY_MB 20971520

static const char* filePaths[] = 
{
    "out/1.txt",
    "out/2.txt",
    "out/3.txt",
    "out/4.txt",
    "out/5.txt"
};

static const char* outputPath = "out/out.txt";
char buffer[TWENTY_MB];

#ifdef __linux__

size_t bufsize = 0;

void  fileReader(pthread_mutex_t*);
void* fileWriter(void*);

void fileReader(pthread_mutex_t* mutex)
{
    int fd, ret;
    struct aiocb aio_info = {0};

    aio_info.aio_buf = &buffer;
    aio_info.aio_nbytes = TWENTY_MB;

    for (int i = 0; i < 5; ++i)
    {
	    fd = open(filePaths[i], O_RDONLY);

	    if (fd < 0) 
	    {
	        fprintf(stderr, "Opening file %s error", filePaths[i]);
	        exit(EXIT_FAILURE);
	    }

	    pthread_mutex_lock(mutex);

	    aio_info.aio_fildes = fd;
	    ret = aio_read(&aio_info);

	    if (ret < 0)
	    {
	        perror("aio_read");
	        close(fd);
	        exit(EXIT_FAILURE);
	    }
	    printf("File %s is reading.", filePaths[i]);

	    while (aio_error( &aio_info ) == EINPROGRESS)
	    {
	        putchar('.');
	        fflush(stdout);
	        Sleep(SHORT_SLEEP);			
	    }

	    if ((ret = aio_return(&aio_info)) < 0)
	    {
	        perror("aio_read");
	        close(fd);
	        exit(EXIT_FAILURE);
	    }

	    puts("Success");
	    bufsize = ret;
	    pthread_mutex_unlock(mutex);
	    close(fd);

	    Sleep(LONG_SLEEP);
    }
	
}

void* fileWriter(void* mutex)
{
    int fd, ret;
    struct aiocb aio_info = {0};

    aio_info.aio_buf = buffer;
    fd = open(outputPath, O_CREAT | O_WRONLY);
    if (fd < 0) 
    {
	    fputs("Opening output file error", stderr);
	    exit(EXIT_FAILURE);
    }
    aio_info.aio_fildes = fd;

    while (bufsize == 0);
    Sleep(LONG_SLEEP);

    for (int i = 0; i < 5; ++i)
    {
	    pthread_mutex_lock((pthread_mutex_t*)mutex);
	    aio_info.aio_nbytes = bufsize;
	    ret = aio_write(&aio_info);
	    if (ret < 0)
	    {
	        perror("aio_write");
	        close(fd);
	        exit(EXIT_FAILURE);
	    }
	    printf("Writing to file %s.", outputPath);
	    while (aio_error(&aio_info) == EINPROGRESS)
	    {
	        putchar('.');
	        fflush(stdout);
	        Sleep(SHORT_SLEEP);
	    }
	    if ((ret = aio_return(&aio_info)) < 0)
	    {
	        perror("aio_write");
	        close(fd);
	        exit(EXIT_FAILURE);
	    }

	    puts("Success");
	    pthread_mutex_unlock(mutex);
	    aio_info.aio_offset += ret;

	    Sleep(LONG_SLEEP);
    }

    close(fd);

    return NULL;
}

#elif _WIN32

DWORD bufsize = 0;

void fileReader(CRITICAL_SECTION* critical_section);
void fileWriter(LPVOID critical_section);

void fileReader(CRITICAL_SECTION* critical_section)
{
    HANDLE file;
    for (int i = 0; i < 5; i++)
    {
	    file = CreateFileA(filePaths[i], GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);	
	    if (file == INVALID_HANDLE_VALUE)
	    {
	        fputs("Opening file error", stderr);
	        exit(EXIT_FAILURE);
	    }
	
	    OVERLAPPED overlapped = {0};
	    EnterCriticalSection(critical_section);

	    if (ReadFile(file, buffer, TWENTY_MB, &bufsize, (LPOVERLAPPED)&overlapped) != 0)
	    {
	        fputs("Reading file error", stderr);
	        exit(GetLastError());
	    }
	    printf("Reading file %s...    ", filePaths[i]);
	
	    if (GetOverlappedResult(file, &overlapped, &bufsize, TRUE) == 0)
	    {
	        fputs("Reading file error", stderr);
	        exit(EXIT_FAILURE);
	    }
	    puts("Success");

	    LeaveCriticalSection(critical_section);
	    CloseHandle(file);
	    Sleep(LONG_SLEEP);
    }

}

void fileWriter(LPVOID p_critical_section)
{
    CRITICAL_SECTION* critical_section = (CRITICAL_SECTION*)p_critical_section;
        
    HANDLE file = CreateFileA(outputPath, FILE_ALL_ACCESS, 0, NULL, CREATE_ALWAYS, FILE_FLAG_OVERLAPPED, NULL);

    if (file == INVALID_HANDLE_VALUE)
    {
	    fputs("Opening output file error", stderr);
	    exit(EXIT_FAILURE);
    }

    while (bufsize == 0);
    Sleep(SHORT_SLEEP);

    for (int i = 0; i < 5; i++)
    {
	    EnterCriticalSection(critical_section);
	    DWORD filePtr = SetFilePointer(file, 0, NULL, FILE_END);
	    OVERLAPPED overlapped = {0};
	    overlapped.Offset = filePtr;
	    
	    if (!WriteFile(file, buffer, bufsize, NULL, &overlapped) && GetLastError() != ERROR_IO_PENDING)
	    {
	        fputs("Writing to file error", stderr);
	        exit(EXIT_FAILURE);
	    }
	    printf("Writing to file %s...   ", outputPath);

	    if (GetOverlappedResult(file, &overlapped, &bufsize, TRUE) == 0)
	    {
	        fputs("Reading file error", stderr);
	        exit(EXIT_FAILURE);
	    }
	    puts("Success");
	    LeaveCriticalSection(critical_section);

	    Sleep(LONG_SLEEP);
    }
    CloseHandle(file);
    return;
}

#endif