#include "System.h"

double avg = 0.0;
uint64 num = 0;

int main(int argc, char *argv[])
{
	Timer t;
	t.start( );
	System sys;
	sys.SayHello();
	sys.HandleArguments(argc, argv);
	sys.ExportDBCs();
	printf( "%f", t.getElapsedTime( ) );
	getchar();
	printf("Done!\n");
	return 0;
}

