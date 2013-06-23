//#define saco_para_prueba
#ifndef saco_para_prueba
#include <iostream>
#include "cliente_controlador.h"
#include "clientInterface.h"
#include "common_base_de_datos.h"
#include <gtkmm.h>
#include <glibmm.h>
#include <syslog.h>

/**
 * @file clientMain.cpp
 * @brief Main de cliente
 */
int main (int argc, char** argv)
{
	setlogmask(LOG_UPTO(S_MENOR_PRIOR));
	openlog(argv[0], S_OPTION, S_FACILITY);
	try
	{
		umask(0077); // Permisos
		Gtk::Main kit(argc,argv);
		ClientInterface interface;
		interface.correr();
	}
	catch (Glib::Exception &e)
	{
		syslog(LOG_EMERG, "%s", e.what().c_str());
	}
	catch (std::exception &e)
	{
		syslog(LOG_EMERG, "%s", e.what());
	}
	closelog();
	return 0;
}
#endif
