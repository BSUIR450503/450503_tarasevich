#include "NewLib.h"

int main(int argc, char* argv[])
{
	ProcessFamily proc;
	PID myPid;
	proc.child(myPid, argc, argv);

	return 0;
}