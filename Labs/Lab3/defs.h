#pragma once

#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#define BUFFER_SIZE 256
const char CLOSE_MESSAGE[] = "exit\n";

#ifdef __linux__

#include <unistd.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/types.h>

#define CLIENT_SEMAPHORE 0
#define SERVER_SEMAPHORE 1
#define Sleep usleep
#define SLEEP_TIME 50000
void linux_handler(int, char**);
void WaitSemaphore(int, int);
void ReleaseSemaphore(int, int);

using std::cout;
using std::endl;

#elif _WIN32

#include <Windows.h>
#include <conio.h>
#define SLEEP_TIME 50

const char PIPE_PATH[] = "\\\\.\\pipe\\lab3";
const char SERVER_SEM[] = "server";
const char CLIENT_SEM[] = "client";
typedef HANDLE sem_t;

sem_t create_semaphore(const char*);
sem_t open_semaphore(const char*);
HANDLE create_pipe();
HANDLE init_client();
void server_handler();
void client_handler();

#endif