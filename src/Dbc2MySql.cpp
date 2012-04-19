#include "System.h"

int main(int argc, char *argv[])
{
	System sys;

	sys.SayHello();
	sys.HandleArguments(argc, argv);
	sys.ExportDBCs();

	printf("Done!\n");
	return 0;
}

