#ifndef CLIENTINTERFACE_H_
#define CLIENTINTERFACE_H_

#include <gtkmm.h>
#include <glibmm.h>
#include "cliente_controlador.h"

#define USER_GLADE "user.glade"
#define ARCH_PREFS_CLI "client.cfg"
#define IP_DEF "127.0.0.1"
#define PORT1_DEF "12700"
#define PORT2_DEF "12701"
#define POLLING_DEF "5"

/**
 * @class ClientInterface
 * @brief Clase encargada de la interface usuario del cliente
 */
class ClientInterface
{
public:
	ClientInterface();
	/**
	 * @brief Pone a "correr" la ventana, mostrandola.
	 */
	void correr();
private:
	Gtk::Dialog* window;
	Glib::RefPtr<Gtk::Builder> builder;
	ClienteControlador cli;
	/**
	 * @brief Carga las preferencias del archivo de preferencias
	 */
	void cargarPreferencias();
	/**
	 * @brief Metodo a llamar al apretarse el boton de login
	 */
	void login();
	/**
	 * @brief Metodo a llamar al cerrarse la aplicacion
	 */
	void cerrar();
};

#endif /* CLIENTINTERFACE_H_ */
