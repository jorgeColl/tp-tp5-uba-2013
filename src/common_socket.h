#ifndef COMMON_SOCKET_H_
#define COMMON_SOCKET_H_

#include <string>
#include <sstream>

using namespace std;

/**
 * @class Socket common_socket.h "common_socket.h"
 * @brief Encapsula el socket file descriptor y asociados
 */
class Socket
{
	public:
		/**
		 * @brief Constructor por defecto
		 */
		Socket();
		/**
		 * @brief Crea una instancia de socket asociada al file descriptor
		 */
		Socket(int sockfd);
		/**
		 * @brief conecta el socket a un puerto y adress dado
		 * @throws Excepciones si la conexion falla
		 */
		void conectar(const char* address, const char* puerto);
		/**
		 * @brief pone el socket a escuchar en el puerto pedido
		 * @param port Puerto a usar
		 * @param maxCola Numero maximo de conexiones a tener esperando
		 * @throws Excepciones si la conexion falla
		 */
		void escuchar(const char* port, int maxCola);
		/**
		 * @brief Acepta una conexion
		 * @return El socket file descriptor asociado a la nueva conexion
		 */
		int aceptar();
		/**
		 * @brief encapsula el send() default
		 */
		int enviar(void *msg, size_t len);
		/**
		 * @brief encapsula el recv() default
		 */
		int recibir(void *msg, size_t len);
		/**
		 * @brief Cierra el puerto
		 */
		void cerrar();
		/**
		 * @brief Envia hasta que logre enviarse len
		 * @return True si pudo enviarse, false en otro caso
		 */
		bool enviarLen(void *msg, size_t len);
		/**
		 * @brief Recibe hasta que logre recibirse len. El "sobrante" si existe queda en este socket
		 * @return True si pudo recibirse, false en otro caso
		 */
		bool recibirLen(void *msg, size_t len);
		/**
		 * @var Socket file descriptor asociado
		 */
		int sockfd;
	private:
		stringstream sobrantes;
};

#endif /* COMMON_SOCKET_H_ */

