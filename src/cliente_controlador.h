#ifndef CLIENTE_H_
#define CLIENTE_H_
#include <string>
#include <vector>
#include "common_controlador.h"

/**
 * @class Cliente cliente_controlador.h
 * @brief Clase que se encargarÃ¡ de las acciones generales que puede hace un cliente
 */
class ClienteControlador : public Controlador
{
private:
	SocketProt sock1;
	SocketProt sock2;
	string dir;
	BaseDeDatos base_de_datos;
	bool terminar;

	vector<Modificacion> pedir_y_comparar_indices();
public:
	ClienteControlador();
	/**
	 * @brief IntentarÃ¡ iniciar sesion con el usuario y contraseÃ±a ingresados, en el servidor y los puertos indicados
	 * @details Se comunicarÃ  con el servidor y este le responderÃ¡ si el usuario y contraseÃ±a son correctos
	 * @return True si el login fue exitoso, false en caso contrario
	 */
	bool login(std::string server, std::string puerto1, std::string puerto2, std::string usuario, std::string contrasenia);

	void logout();

	void start();
	/**@brief pide lista de modificaciones al servidor
	 */
	std::vector<Modificacion> recibir_modificaciones();
};

#endif /* CLIENTE_H_ */
