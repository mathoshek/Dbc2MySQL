#include "System.h"

int main(int argc, char *argv[])
{
	System sys;
   //FILE *stream ;
   //if((stream = freopen("file.txt", "w", stdout)) == NULL)
   //   exit(-1);

	sys.SayHello();
	sys.HandleArguments(argc, argv);
	sys.ExportDBCs();

	printf("Done!\n");
	return 0;
}

