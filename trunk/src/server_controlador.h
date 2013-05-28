#ifndef SERVER_H_
#define SERVER_H_

#include "common_controlador.h"
#include <string>

/** @class Server server_controlador.h
 * @brief clase que se encarga de las acciones que puede hacer un server
 */
class ServerControlador: public Controlador {
	std::string dir;
	int puerto;
public:
	ServerControlador(std::string puerto, std::string directorio);

	/** @brief indica al thread que acepta las conexiones que termine de ejecutarse */
	void stop();

	void start();

	bool enviar_modificaciones();
	/** @brief devuelve true o false de acuerdo a si el nombre de usuario y contraseña son correctos
	 */
	bool login_cliente(std::string& mensaje_de_login);

};



#endif /* SERVER_H_ */
