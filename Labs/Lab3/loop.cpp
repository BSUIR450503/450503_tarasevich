#include "defs.h"

#ifdef _WIN32

void server_handler() {
	puts("  ==================== SERVER ====================");
	HANDLE namedPipe = create_pipe();
	DWORD bytesToRead;
	DWORD bytesToWrite;
	sem_t hServer = create_semaphore(SERVER_SEM);
	sem_t hClient = create_semaphore(CLIENT_SEM);
	char buffer[BUFFER_SIZE];
	while (1) {
		ReleaseSemaphore(hClient, 1, NULL);
		WaitForSingleObject(hServer, INFINITE);
		if (ReadFile(namedPipe, buffer, BUFFER_SIZE, &bytesToRead, NULL)) {
			printf("Client: ");
			for (int i = 0; i < strlen(buffer); i++) {
				printf("%c", buffer[i]);
				Sleep(SLEEP_TIME);
			}
			if (!strcmp(buffer, CLOSE_MESSAGE)) {
				CloseHandle(namedPipe);
				CloseHandle(hServer);
				return;
			}
		}
		printf("Server: ");
		fgets(buffer, BUFFER_SIZE, stdin);

		if (!WriteFile(namedPipe, buffer, BUFFER_SIZE, &bytesToWrite, NULL))
			break;
		if (!strcmp(buffer, CLOSE_MESSAGE)) {
			CloseHandle(namedPipe);
			CloseHandle(hServer);
			return;
		}
		ReleaseSemaphore(hClient, 1, NULL);
	}
	CloseHandle(namedPipe);
	CloseHandle(hServer);
	return;
}

void client_handler() {
	puts("  ==================== CLIENT ====================");
	HANDLE namedPipe = CreateFile(PIPE_PATH,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		0,
		NULL);
	if (namedPipe == INVALID_HANDLE_VALUE) {
		printf("Can't create pipe. Press any key to exit...");
		_getch();
		exit(0);
	}
	DWORD bytesToRead;
	DWORD bytesToWrite;
	sem_t hServer = open_semaphore(SERVER_SEM);
	sem_t hClient = open_semaphore(CLIENT_SEM);
	ReleaseSemaphore(hServer, 1, NULL);
	char buffer[BUFFER_SIZE];
	while (1) {
		WaitForSingleObject(hClient, INFINITE);
		if (ReadFile(namedPipe, buffer, BUFFER_SIZE, &bytesToRead, NULL)) {
			printf("Server: ");
			for (int i = 0; i < strlen(buffer); i++) {
				printf("%c", buffer[i]);
				Sleep(SLEEP_TIME);
			}
			if (!strcmp(buffer, CLOSE_MESSAGE)) {
				CloseHandle(hClient);
				return;
			}
		}
		
		printf("Client: ");
		fgets(buffer, BUFFER_SIZE, stdin);
		if (!WriteFile(namedPipe, buffer, BUFFER_SIZE, &bytesToWrite, NULL))
			break;
		if (!strcmp(buffer, CLOSE_MESSAGE)) {
			ReleaseSemaphore(hServer, 1, NULL);
			CloseHandle(hClient);
			return;
		}
		ReleaseSemaphore(hServer, 1, NULL);
	}

	return;
}

HANDLE init_client() {
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	char PATH[] = "Lab3_SSoC2.exe client";

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcess(NULL, PATH, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
		printf("CreateProcess failed (%d).\n", GetLastError());
		getchar();
		exit(1);
	}
	return pi.hProcess;
}

HANDLE create_pipe() {
	HANDLE pipe = CreateNamedPipe(PIPE_PATH,    // Name
		PIPE_ACCESS_DUPLEX, // OpenMode
		PIPE_TYPE_MESSAGE | PIPE_READMODE_BYTE | PIPE_NOWAIT, // PipeMode
		3,            // MaxInstances
		1024,         // OutBufferSize
		1024,         // InBuffersize
		1,            // TimeOut
		NULL);        // Security
	if (pipe == INVALID_HANDLE_VALUE) {
		printf("Could not create the pipe\n");
		exit(1);
	}
	ConnectNamedPipe(pipe, NULL);
	return pipe;
}

sem_t create_semaphore(const char *name) {
	SECURITY_ATTRIBUTES attr;
	sem_t handle;
	attr.bInheritHandle = TRUE;
	attr.nLength = sizeof(attr);
	attr.lpSecurityDescriptor = NULL;
	handle = CreateSemaphore(&attr, 0, 1, name);
	return handle;
}

sem_t open_semaphore(const char *name) {
	return OpenSemaphore(SEMAPHORE_ALL_ACCESS, TRUE, name);
}

#elif __linux__

void linux_handler(int argc, char **argv) {
	srand(time(0));
	key_t key = rand() % 10000 + 1;
	system("clear");
	int shm_id = shmget(key, BUFFER_SIZE, IPC_CREAT | IPC_EXCL | 0666);

	if (shm_id < 0) {
		if ((shm_id = shmget(key, BUFFER_SIZE, 0)) < 0) {
			printf("shmget error\n");
			exit(1);
		}
	}

	int sem_id = semget(key, 2, IPC_CREAT | 0666);
	semctl(sem_id, 0, SETALL, 0);

	if (sem_id < 0) {
		printf("Semaphores is not created.");
		exit(1);
	}

	if (argc != 2) {

		int pid = fork();
		if (pid == -1) {
			printf("New process is not created\n");
			exit(1);
		} else if (pid == 0) {
			execlp("gnome-terminal", "gnome-terminal", "-x", argv[0], "1", NULL);
		} else {
			void *buffer = shmat(shm_id, NULL, 0);
			cout << "  ==================== SERVER ====================" << endl;
			while (1) {
				char messageToClient[BUFFER_SIZE];
				fflush(stdin);
				cout << "Server: ";
				fgets(messageToClient, BUFFER_SIZE, stdin);
				memcpy(buffer, messageToClient, BUFFER_SIZE);
				ReleaseSemaphore(sem_id, CLIENT_SEMAPHORE);
				if (!strcmp(messageToClient, CLOSE_MESSAGE)) {
					shmdt(buffer);
					shmctl(shm_id, IPC_RMID, NULL);
					system("clear");
					return;
				}
				WaitSemaphore(sem_id, SERVER_SEMAPHORE);
				char messageFromClient[BUFFER_SIZE];
				memcpy(messageFromClient, buffer, BUFFER_SIZE);
				if (!strcmp(messageFromClient, CLOSE_MESSAGE)) {
					shmdt(buffer);
					shmctl(shm_id, IPC_RMID, NULL);
					system("clear");
					return;
				}
				cout << "Client: ";
				for (int i = 0; messageFromClient[i] != '\0'; i++) {
					putchar(messageFromClient[i]);
					fflush(stdout);
					Sleep(SLEEP_TIME);
				}
			}
		}
	} else if (argc == 2) {
		void *buffer = shmat(shm_id, NULL, 0);
		cout << "  ==================== CLIENT ====================" << endl;
		while (1) {
			char messageFromServer[BUFFER_SIZE];
			WaitSemaphore(sem_id, CLIENT_SEMAPHORE);
			memcpy(messageFromServer, buffer, BUFFER_SIZE);
			if (!strcmp(messageFromServer, CLOSE_MESSAGE)) {
				shmdt(buffer);
				shmctl(shm_id, IPC_RMID, NULL);
				system("clear");
				return;
			}
			cout << "Server: ";
			for (int i = 0; messageFromServer[i] != '\0'; i++) {
				putchar(messageFromServer[i]);
				fflush(stdout);
				Sleep(SLEEP_TIME);
			}

			char messageForServer[BUFFER_SIZE];
			fflush(stdin);
			cout << "Client: ";
			fgets(messageForServer, BUFFER_SIZE, stdin);
			memcpy(buffer, messageForServer, BUFFER_SIZE);
			ReleaseSemaphore(sem_id, SERVER_SEMAPHORE);
			if (!strcmp(messageForServer, CLOSE_MESSAGE)) {
				shmdt(buffer);
				shmctl(shm_id, IPC_RMID, NULL);
				system("clear");
				return;
			}
		}
	}
}

void WaitSemaphore(int sem_id, int num) {
	struct sembuf buf;
	buf.sem_op = -1;
	buf.sem_flg = SEM_UNDO;
	buf.sem_num = num;
	semop(sem_id, &buf, 1);
}

void ReleaseSemaphore(int sem_id, int num) {
	struct sembuf buf;
	buf.sem_op = 1;
	buf.sem_flg = SEM_UNDO;
	buf.sem_num = num;
	semop(sem_id, &buf, 1);
}

#endif