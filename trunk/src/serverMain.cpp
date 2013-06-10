//#define server_main
#ifndef server_main

#include <sys/stat.h>	// Umask
#include <iostream>
#include "server_accepter.h"

#define ARCH_PREFS_SERV "server.cfg"
#define PUERTO1_DEF "12700"
#define PUERTO2_DEF "12701"
#define DIR_DEF "./serverDir"

void cargar(string &dir, string &puerto1, string &puerto2)
{
	ifstream arch(ARCH_PREFS_SERV);
	if (arch.is_open())
	{
		arch >> dir;
		arch >> puerto1;
		arch >> puerto2;
	}
	else
	{
		dir = DIR_DEF;
		puerto1 = PUERTO1_DEF;
		puerto2 = PUERTO2_DEF;
	}
	arch.close();
}

/**
 * @file serverMain.cpp
 * @brief Main del servidor
 */
int main (int argc, char** argv)
{
	try
	{
		umask(0077); // Permisos
		string dir, p1, p2;
		cargar(dir, p1, p2);
		cout << "Directorio: " << dir << endl;
		cout << "Puerto 1: " << p1 << endl;
		cout << "Puerto 2:" << p2 << endl;
		Accepter acp(dir.c_str(), p1.c_str(), p2.c_str());
		// Aca manda a ejecutar al accepter en otro thread
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
