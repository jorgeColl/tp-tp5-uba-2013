/*
 * server_base_de_datos_usuario.h
 *
 *  Created on: 28/05/2013
 *      Author: jorge
 */

#ifndef SERVER_BASE_DE_DATOS_USUARIO_H_
#define SERVER_BASE_DE_DATOS_USUARIO_H_
#include <fstream>
#include <string>
#include <map>
#include <stdexcept> 	// Excepciones genericas
#include <cerrno>
#include <cstring>

/** en principio solo es un hash en ram*/
class BaseDeDatosUsuario {
	const char* dir;
	std::map<std::string ,std::string> usu_contr;
public:
	BaseDeDatosUsuario(const char* dir);
	/**@brief abre y carga en memoria la base de datos de usuario y su respectiva contraseña */
	void abrir();
	/**@brief Verifica si el usuario y contraseña ingresados pertenecen a un usuario de la base de datos */
	bool usuario_contrasenia_correcto(const char* usu, const char* contrasenia);
	/**@brief Agrega un usuario a la base de datos */
	void agregar_usuario(const char* usuario, const char* contrasenia);
	/**@brief Elimina un usuari de la base de datos */
	void eliminar_usuario(const char* usuario);
	/**@brief guarda los datos que estan en la ram en disco*/
	void guardar_a_disco();
	/**@brief POSIBLE FUNCION a implementar en el futuro, lee otra vez del archivo y agrega
	 * los nuevos usuarios que encuentre*/
	void actualizar();
};



#endif /* SERVER_BASE_DE_DATOS_USUARIO_H_ */
