#ifndef COMMON_SOCKET_PROT_H_
#define COMMON_SOCKET_PROT_H_

#include <fstream>
#include "common_socket.h"
#include "common_modificacion.h"

using namespace std;

enum Packet {LOGIN_OK, ARCH_MODIF, ARCH_NUEVO, ARCH_ELIM};

/**
 * @class SocketProt common_socket_prot.h "common_socket_prot.h"
 * @brief Encapsula el socket file descriptor y asociados
 */
class SocketProt : public Socket
{
public:
	/**
	 * @brief Envia un byte con flags seteados que indica un mensaje, o el tipo de mensaje que vendra
	 * @param flag Byte que contiene los flags
	 * @return True en caso de exito, false si falla
	 */
	bool enviar_flag(const Packet flag);
	/**
	 * @brief Recibe un byte con flags seteados que indica un mensaje, o el tipo de mensaje que vendra
	 * @param flag Byte donde se almacenaran los flags
	 * @return True en caso de exito, false si falla
	 */
	bool recibir_flag(Packet &flag);
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
	bool enviar_pedazo_archivo(ifstream &arch, size_t offset, size_t len);
	/**
	 * @brief Recibe un pedazo de un archivo
	 * @param arch Archivo donde se guardara
	 * @param offset Offset en bytes en el archivo
	 * @param len Longitud del pedazo a recibir
	 * @return True en caso de exito, false si falla
	 */
	bool recibir_pedazo_archivo(ofstream &arch, size_t offset, size_t len);
};

#endif /* COMMON_SOCKET_PROT_H_ */
