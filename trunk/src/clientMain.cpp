//#define parte_grafica
#ifndef parte_grafica
#include <iostream>
#include "cliente_controlador.h"
#include "clientInterface.h"
#include "common_base_de_datos.h"

/**
 * @file clientMain.cpp
 * @brief Main de cliente
 * 1er parametro: nombre_de_usuario
 * 2do parametro: contraseña
 * 3er parametro: ip_del_server
 * 4to parametro: puerto
 */
int main (int argc, char** argv)
{
	try
	{
		BaseDeDatos b;
		Gtk::Main kit(argc,argv);
		ClientInterface interface(argc, argv);
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
