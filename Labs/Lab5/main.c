#include "defines.h"

int main(int argc, char const *argv[])
{
	connectLibrary();
	createThread();
	disconnectLibrary();
	return 0;
}