/*
 * server.h
 *
 *  Created on: 19/05/2013
 *      Author: jorge
 */

#ifndef SERVER_H_
#define SERVER_H_
/** @class Server server_controlador.h
 * @brief clase que se encarga de las acciones que puede hacer un server
 */
class ServerControlador {
std::string dir;
std::string puerto;
public:
	ServerControlador(std::string puerto, std::string directorio);
	/** @brief inicia un thread que se encarga de acceptar las conexiones entrantes */
	void start();
	/** @brief indica al thread que acepta las conexiones que termine de ejecutarse */
	void stop();
	bool enviar_modificaciones();
	/** @brief devuelve true o false de acuerdo a si el nombre de usuario y contraseña son correctos
	 */
	bool login_cliente(std::string& mensaje_de_login);
};



#endif /* SERVER_H_ */
