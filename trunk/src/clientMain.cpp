//#define saco_para_prueba
#ifndef saco_para_prueba
#include <iostream>
#include "cliente_controlador.h"
#include "clientInterface.h"
#include "common_base_de_datos.h"
#include <gtkmm.h>
#include <glibmm.h>

/**
 * @file clientMain.cpp
 * @brief Main de cliente
 */
int main (int argc, char** argv)
{
	try
	{
		umask(0077); // Permisos
		Gtk::Main kit(argc,argv);
		ClientInterface interface;
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
