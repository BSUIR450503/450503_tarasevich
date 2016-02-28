#include "NewLib.h"

int main(int argc, char* argv[])
{
	MyProc proc;
	PID myPid;
	proc.child(myPid, argc, argv);

	return 0;
}