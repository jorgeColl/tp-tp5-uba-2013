//#define server_main
#ifndef server_main

#include <sys/stat.h>	// Umask
#include <iostream>
#include "server_accepter.h"
#include <syslog.h>
#include <sys/stat.h>
#include "defines.h"

/**
 * @file serverMain.cpp
 * @brief Main del servidor
 */

/**
 * @brief Carga de un archivo las preferencias, y si no existe el archivo usa unas por defecto
 * @param dir En este string se guardara el directorio
 * @param puerto1 En este string se guardara el puerto 1
 * @param puerto2 En este string se guardara el puerto 2
 */
void cargarPrefs(string &dir, string &puerto1, string &puerto2)
{
	ifstream arch(ARCH_PREFS_SERV);
	if (arch.is_open())
	{
		arch >> puerto1;
		arch >> puerto2;
		//arch >> dir;
		arch.get();
		getline(arch,dir);
	}
	else
	{
		dir = DIR_DEF_SERV;
		puerto1 = PUERTO1_DEF;
		puerto2 = PUERTO2_DEF;
	}
	arch.close();
}

int main (int argc, char** argv)
{
	try
	{
		umask(0077); // Permisos
		string dir, p1, p2;
		cargarPrefs(dir, p1, p2);
		syslog(LOG_DEBUG, "Directorio: %s", dir.c_str());
		syslog(LOG_DEBUG, "Puerto 1: %s", p1.c_str());
		syslog(LOG_DEBUG, "Puerto 2: %s", p2.c_str());
		mkdir(dir.c_str(), 0700); // Si no existia la carpeta, se crea.
		Accepter acp(dir.c_str(), p1.c_str(), p2.c_str());
		// Aca manda a ejecutar al accepter en otro thread
		acp.start();
		string buffer;
		while(buffer != "q")
		{
			cin >> buffer;
		}
		acp.stop();
		acp.join();
	}
	catch(exception &e)
	{
		syslog(LOG_EMERG, "Error fatal: %s", e.what());
	}
	return 0;

}
#endif
