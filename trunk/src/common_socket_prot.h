#ifndef COMMON_SOCKET_PROT_H_
#define COMMON_SOCKET_PROT_H_

#include <fstream>
#include <stdint.h>
#include "common_socket.h"
#include "common_modificacion.h"
#include "defines.h"
#include "common_mutex.h"
#include "hashlib/hl_md5wrapper.h"

using namespace std;

enum PacketID {ZERO = 0, OK, YA_APLICADA, CONFLICTO, FAIL, LOGIN, LOGOUT, MODIFICACION, PEDIDO_ARCHIVO_ENTERO,
	PEDIDO_ARCHIVO_EDICIONES, PEDIDO_INDICE};

/**
 * @class SocketProt common_socket_prot.h "common_socket_prot.h"
 * @brief Encapsula el socket file descriptor y asociados
 */
class SocketProt : public Socket
{
	//string bufffer;
	string contrasenia;
	MD5 md5;
	HL_MD5_CTX ctx;
public:
	/*static Mutex mutex_cant_transmitida;
	static Mutex mutex_cant_recibida;
	static size_t cantidad_transmitida;
	static size_t cantidad_recibida;
	static size_t get_and_reset_cantidad_recibida();
	*/
	SocketProt();
	SocketProt(int socketfd);

	virtual int enviar(void *msg, size_t len);

	virtual int recibir(void *msg, size_t len);
	/**
	 * @brief Envia un byte con flags seteados que indica un mensaje, o el tipo de mensaje que vendra
	 * @param flag Byte que contiene los flags
	 * @return True en caso de exito, false si falla
	 */
	void enviar_flag(const PacketID flag);
	/**
	 * @brief Recibe un byte con flags seteados que indica un mensaje, o el tipo de mensaje que vendra
	 * @param flag Byte donde se almacenaran los flags
	 * @return True en caso de exito, false si falla
	 */
	void recibir_flag(PacketID &flag);
	/**
	 * @brief Envia un mensaje, pero le encaja un prefijo de longitud adelante
	 * @param msg Mensaje a enviar
	 * @param bytes_para_prefijo cant de bytes a enviar en el mensaje como prefijo
	 * @return True en caso de exito, false si falla
	 */
	void enviar_msg_c_prefijo(const string &msg, uint8_t bytes_para_prefijo);
	/**
	 * @brief Recibe un mensaje, esperando que tenga un prefijo de longitud adelante
	 * @param msg Mensaje a enviar
	 * @param bytes_para_prefijo cantidad de bytes a leer del mensaje que se interpretarán como prefijo
	 * @return True en caso de exito, false si falla
	 */
	void recibir_msg_c_prefijo(string &msg, uint8_t bytes_para_prefijo);
	/**
	 * @brief Envia una modificacion a travez del socket
	 * @param modif Modificacion que sera enviada
	 * @return True en caso de exito, false si falla
	 */
	void enviar_modif(const Modificacion &modif);
	/**
	 * @brief Recibe una modificacion a travez del socket
	 * @param modif Modificacion donde se guardara
	 * @return True en caso de exito, false si falla
	 */
	void recibir_modif(Modificacion &modif);
	/**
	 * @brief Envia un pedazo de un archivo
	 * @param arch Archivo a utilizar
	 * @param offset Offset en bytes en el archivo
	 * @param len Longitud del pedazo a enviar a partir del offset
	 * @return True en caso de exito, false si falla
	 */
	void enviar_pedazo_archivo(istream &arch, off_t offset, off_t len);
	/**
	 * @brief Envia un archivo entero
	 * @param arch Archivo a enviar
	 * @pre El archivo esta abierto en modo lectura binaria
	 */
	void enviar_archivo(istream &arch);
	/**
	 * @brief Recibe un pedazo de un archivo
	 * @param arch Archivo donde se guardara
	 * @param offset Offset en bytes en el archivo
	 * @return True en caso de exito, false si falla
	 */
	void recibir_pedazo_archivo(ostream &arch, off_t offset);
	/**
	 * @brief Recibe un archivo entero
	 * @param arch Archivo a recibir
	 * @return True en caso de exito, false si falla
	 * @pre El archivo esta abierto en modo escritura binaria
	 */
	void recibir_archivo(ostream &arch);
	/**
	 * @brief Envia los hashes de los bloques del archivo y luego lo que se le pida
	 * @param arch Archivo del cual enviar los hashes y las partes
	 */
	void enviar_edicion(istream &arch);
	/**
	 * @brief Recibe los hashes de los bloques, pide lo que necesite y vuelca el resultado en arch_temp
	 * @param arch_orig Archivo original al cual se le aplicaran las ediciones
	 * @param arch_temp Archivo donde se escribira la union entre lo recibido y lo del archivo original
	 */
	void recibir_edicion(istream &arch_orig, ostream &arch_temp);
	/**@brief firma un mensaje  */
	void enviar_firma();
	/**@brief verifica si el mensaje está correctamente firmado */
	void comprobar_firma();

	//void guardar_cant_transmitida(size_t cantidad);

	//void guardar_cant_recibida(size_t cantidad);
};

#endif /* COMMON_SOCKET_PROT_H_ */
