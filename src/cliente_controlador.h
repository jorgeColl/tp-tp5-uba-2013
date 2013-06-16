#ifndef CLIENTE_H_
#define CLIENTE_H_
#include <string>
#include <vector>
#include "common_controlador.h"
#include "common_thread.h"
#include "cliente_notificador.h"
#include "common_mutex.h"
using namespace std;

#define NOM_IDX_SERV ".auserver"
#define POLLING_DEFAULT 5

/**
 * @class Cliente cliente_controlador.h
 * @brief Clase que se encargara de las acciones generales que puede hace un cliente
 */
class ClienteControlador : public Controlador, public Thread
{
private:
	size_t delay_polling;
	ClienteNotificador notificador;
	Mutex mutexCambios;
	/**
	 * @brief Pide al servidor su indice y lo compara con los archivos locales y con el indice local
	 * @return Modificaciones que deberian aplicarse para estar sincronizado con el servidor y ademas pasarle los nuevos cambios
	 */
	list<Modificacion> pedir_y_comparar_indices();

	/**
	 * @brief Metodo a sobrecargar para el uso de threads por herencia
	 */
	void ejecutar();
public:
	ClienteControlador();
	/**
	 * @brief IntentarÃ¡ iniciar sesion con el usuario y contraseÃ±a ingresados, en el servidor y los puertos indicados
	 * @details Se comunicara  con el servidor y este le responderÃ¡ si el usuario y password son correctos
	 * @return True si el login fue exitoso, false en caso contrario
	 */
	void login(string server, string puerto1, string puerto2,string usuario,
			string contrasenia, string polling);

	/**
	 * @brief Funcion a utilizar para aplicar una modificacion "externamente"
	 * @details Es muy similar a "aplicar_modificacion" pero lockea el mutex y loguea errores
	 * @param modif Modificacion a aplicar
	 */
	void AplicarNotificacion(Modificacion &modif);

};

#endif /* CLIENTE_H_ */
