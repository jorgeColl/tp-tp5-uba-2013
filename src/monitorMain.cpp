//#define monitor_pepe
#ifndef monitor_pepe
#include <iostream>
#include <gtkmm.h>
#include <glibmm.h>
#include "monitorInterface.h"

/**
 * @file monitorMain.cpp
 * @brief Main de la aplicacion monitor
 */
int main (int argc, char** argv)
{
	try
	{
		umask(0077); // Permisos
		Gtk::Main kit(argc,argv);
		MonitorInterface interface;
		interface.correr();

	}
	catch (Glib::Exception &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
	catch (std::exception &e)
	{
		std::cout << "Error: " << e.what() << std::endl;
	}
	return 0;
}
#endif
