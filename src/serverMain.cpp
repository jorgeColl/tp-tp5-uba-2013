#define server_main
#ifndef server_main

#include <iostream>
#include "server_accepter.h"

#define PUERTO1_DEF "12700"
#define PUERTO2_DEF "12701"
#define DIR_DEF "./serverDir"

int main (int argc, char** argv)
{
	try
	{
		// el parametro 1 tiene que ser el directorio
		Accepter acp(DIR_DEF, PUERTO1_DEF, PUERTO2_DEF);
		// aca manda a ejecutar al accepter en otro thread
		acp.start();
		string buffer;
		while(buffer != "q")
		{
			cin>>buffer;
		}
		acp.stop();
	}
	catch(exception &e)
	{
		cout << "Error: " << e.what() << endl;
	}
	return 0;

}
#endif
