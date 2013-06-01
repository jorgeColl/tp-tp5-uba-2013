#ifndef CLIENTE_H_
#define CLIENTE_H_
#include <string>
#include <vector>
#include "common_controlador.h"
using namespace std;

#define NOM_IDX_SERV ".auserver"
#define POLLING_DEFAULT 30
#define BYTES_CLI_CONT 1

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

	bool terminar;
	size_t delay_polling;

	list<Modificacion> pedir_y_comparar_indices();
public:
	BaseDeDatos base_de_datos;
	ClienteControlador();
	/**
	 * @brief IntentarÃ¡ iniciar sesion con el usuario y contraseÃ±a ingresados, en el servidor y los puertos indicados
	 * @details Se comunicarÃ  con el servidor y este le responderÃ¡ si el usuario y contraseÃ±a son correctos
	 * @return True si el login fue exitoso, false en caso contrario
	 */
	void login(string server, string puerto1, string puerto2,
			string usuario, string contrasenia, string polling);

	void logout();

	void start();
	/**@brief pide lista de modificaciones al servidor
	 */
	std::list<Modificacion> recibir_modificaciones();
};

#endif /* CLIENTE_H_ */
