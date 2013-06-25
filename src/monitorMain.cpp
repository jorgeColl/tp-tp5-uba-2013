#include <iostream>
#include <gtkmm.h>
#include <glibmm.h>
#include "monitorInterface.h"
#include <syslog.h>

/**
 * @file monitorMain.cpp
 * @brief Main de la aplicacion monitor
 */
int main (int argc, char** argv)
{
	openlog(argv[0], LOG_CONS, LOG_USER);
	setlogmask(LOG_DEBUG);
	try
	{
		umask(0077); // Permisos
		Gtk::Main kit(argc,argv);
		MonitorInterface interface;
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
