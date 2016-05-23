#include "defs.h"

int main(int argc, char* argv[]) {

#ifdef _WIN32
	if (argc == 1) {
		init_client();
		server_handler();
	}
	else if (argc == 2) {
		client_handler();
	}
#elif __linux__
	linux_handler(argc, argv);
#endif

	return 0;
}