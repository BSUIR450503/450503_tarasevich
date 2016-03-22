#pragma once

#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_COUNT 10
#define RADIX 10

void print(int);

using std::cout;
using std::endl;

#ifdef _WIN32

#include <Windows.h>
#include <conio.h>

#define _CRT_SECURE_NO_WARNINGS
#define SHORT_SLEEP 1
#define LONG_SLEEP 50
#define HANDLE_MACRO Process

int getch_noblock();

static const char * processes[] = { "1. First process\n", "2. Second process\n", 
                                    "3. Third process\n", "4. Fourth process\n", 
                                    "5. Fifth process\n", "6. Sixth process\n",
                                    "7. Seventh process\n", "8. Eighth process\n", 
                                    "9. Ninth process\n", "10. Tenth process\n" };

#elif __linux__

#include <curses.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

using std::cout;
using std::endl;

void tempSleep(int);

static const char * processes[] = { "1. First process\r\n", "2. Second process\r\n",
                                    "3. Third process\r\n", "4. Fourth process\r\n",
                                    "5. Fifth process\r\n", "6. Sixth process\r\n",
                                    "7. Seventh process\r\n", "8. Eighth process\r\n",
                                    "9. Ninth process\r\n", "10. Tenth process\r\n" };

#define Sleep tempSleep
#define getch_noblock getchar
#define SHORT_SLEEP 1000
#define LONG_SLEEP 50000
#define HANDLE_MACRO pid_t

#endif
