#include "NewLib.h"

int main(int argc, char* argv[])
{
	MyProc proc;
	PID pid;
	proc.parent(pid, argc, argv);

	return 0;
}