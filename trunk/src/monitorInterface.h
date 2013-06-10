#ifndef MONITORINTERFACE_H_
#define MONITORINTERFACE_H_

#include <gtkmm.h>
#include <glibmm.h>
#include <string>
#include "common_base_de_datos_usuario.h"
using namespace std;

#define MONIT_GLADE "server.glade"
#define ARCH_PREFS_SERV "server.cfg"
#define PUERTO1_DEF "12700"
#define PUERTO2_DEF "12701"
#define DIR_DEF "./serverDir"

class MonitorInterface
{
public:
	MonitorInterface();
	/**
	 * @brief Pone a "correr" la ventana, mostrandola.
	 */
	void correr();
	~MonitorInterface();
private:
	Gtk::Window* window;
	Glib::RefPtr<Gtk::Builder> builder;
	BaseDeDatosUsuario* bd_usr;
	/** @brief Carga las preferencias del archivo de preferencias */
	void cargarPreferencias();
	/** @brief Guarda las preferencias del archivo de preferencias */
	void guardarPreferencias();
	/** @brief Registra un nuevo usuario */
	void registrarUsuario();
	/** @brief Borra un usuario */
	void borrarUsuario();
	/** @brief Actualiza el directorio mostrado para la parte de usuarios */
	void mostrarDir();
};

#endif /* MONITORINTERFACE_H_ */
