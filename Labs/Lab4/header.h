#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#ifdef __linux__

#include <unistd.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <string.h>
#include <curses.h>

#define THREADS_NUMBER 10
#define Sleep usleep
#define getch_noblock getch
#define OUT_SLEEP 50000
#define SHORT_SLEEP 1000

#elif _WIN32

#include <Windows.h>
#include <conio.h>

#define THREADS_NUMBER 10
#define OUT_SLEEP 50
#define SHORT_SLEEP 1

#endif

using std::cout;