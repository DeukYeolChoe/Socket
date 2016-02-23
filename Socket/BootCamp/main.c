#include <stdio.h>

#include "log.h"
#include "validation.h"
#include "server.h"

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		makeLog("main: argc != 2\n");
		error();
	}

	if (isNullOrEmpty(argv[1]) == -1)
	{
		makeLog("main: argv is null or empty\n");
		error();
	}
	server(argv[1]);
	return 0;
}
