//#define server_main
#ifndef server_main
#include "server_accepter.h"
#define PUERTO1 "4000"
#define PUERTO2 "4001"
#include <iostream>
int main (int argc, char** argv)
{

	// el parametro 1 tiene que ser el directorio
	Accepter acp(argv[1], PUERTO1, PUERTO2);
	acp.start();
	char buffer[2];
	cin>>buffer;
	while(*buffer != 'q'){
		cin>>buffer;
	}


	return 0;
}
#endif
