#ifndef CLIENTINTERFACE_H_
#define CLIENTINTERFACE_H_

#include <gtkmm.h>
#include <glibmm.h>

#define USER_GLADE "user.glade"

/**
 * @class ClientInterface
 * @brief Clase encargada de la interface usuario del cliente
 */
class ClientInterface
{
public:
	ClientInterface(int argc, char** argv);
	void login();
	void correr();
private:
	Gtk::Dialog* window;
	Glib::RefPtr<Gtk::Builder> builder;
};

#endif /* CLIENTINTERFACE_H_ */
