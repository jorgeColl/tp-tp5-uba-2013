#ifndef COMMON_SOCKET_PROT_H_
#define COMMON_SOCKET_PROT_H_

#include <fstream>
#include <stdint.h>
#include "common_socket.h"
#include "common_modificacion.h"

using namespace std;

#define BYTES_USER_PASS 1

enum PacketID {OK, FAIL, LOGIN, LOGOUT, MODIFICACION, ARCHIVO_ENTERO, ARCHIVO_PARTES, HASHES_BLOQUES, PEDIDO_INDICE};

/**
 * @class SocketProt common_socket_prot.h "common_socket_prot.h"
 * @brief Encapsula el socket file descriptor y asociados
 */
class SocketProt : public Socket
{
public:
	SocketProt();
	SocketProt(int socketfd);
	/**
	 * @brief Envia un byte con flags seteados que indica un mensaje, o el tipo de mensaje que vendra
	 * @param flag Byte que contiene los flags
	 * @return True en caso de exito, false si falla
	 */
	bool enviar_flag(const PacketID flag);
	/**
	 * @brief Recibe un byte con flags seteados que indica un mensaje, o el tipo de mensaje que vendra
	 * @param flag Byte donde se almacenaran los flags
	 * @return True en caso de exito, false si falla
	 */
	bool recibir_flag(PacketID &flag);
	/**
	 * @brief Envia un mensaje, pero le encaja un prefijo de longitud adelante
	 * @param msg Mensaje a enviar
	 * @param bytes_para_prefijo cant de bytes a enviar en el mensaje como prefijo
	 * @return True en caso de exito, false si falla
	 */
	bool enviar_msg_c_prefijo(string &msg, uint8_t bytes_para_prefijo);
	/**
	 * @brief Recibe un mensaje, esperando que tenga un prefijo de longitud adelante
	 * @param msg Mensaje a enviar
	 * @param bytes_para_prefijo cantidad de bytes a leer del mensaje que se interpretarán como prefijo
	 * @return True en caso de exito, false si falla
	 */
	bool recibir_msg_c_prefijo(string &msg, uint8_t bytes_para_prefijo);
	/**
	 * @brief Envia una modificacion a travez del socket
	 * @param modif Modificacion que sera enviada
	 * @return True en caso de exito, false si falla
	 */
	bool enviar_modif(const Modificacion &modif);
	/**
	 * @brief Recibe una modificacion a travez del socket
	 * @param modif Modificacion donde se guardara
	 * @return True en caso de exito, false si falla
	 */
	bool recibir_modif(Modificacion &modif);
	/**
	 * @brief Envia un pedazo de un archivo
	 * @param arch Archivo a utilizar
	 * @param offset Offset en bytes en el archivo
	 * @param len Longitud del pedazo a enviar
	 * @return True en caso de exito, false si falla
	 */
	bool enviar_pedazo_archivo(istream &arch, size_t offset, size_t len);
	/**
	 * @brief Envia un archivo entero
	 * @param arch Archivo a enviar
	 * @pre El archivo esta abierto en modo lectura binaria
	 */
	bool enviar_archivo(istream &arch);
	/**
	 * @brief Recibe un pedazo de un archivo
	 * @param arch Archivo donde se guardara
	 * @param offset Offset en bytes en el archivo
	 * @param len Longitud del pedazo a recibir
	 * @return True en caso de exito, false si falla
	 */
	bool recibir_pedazo_archivo(ostream &arch, size_t offset, size_t len);
	/**
	 * @brief Recibe un archivo entero
	 * @param arch Archivo a recibir
	 * @return True en caso de exito, false si falla
	 * @pre El archivo esta abierto en modo escritura binaria
	 */
	bool recibir_archivo(std::ostream &arch);
	/**@brief firma un mensaje  */
	bool firmar_mensaje(std::string& mensaje, std::string contrasenia);
	/**@brief verifica si el mensaje está correctamente firmado */
	bool comprobar_firma(std::string& mensaje,std::string contrasenia);
};

#endif /* COMMON_SOCKET_PROT_H_ */
