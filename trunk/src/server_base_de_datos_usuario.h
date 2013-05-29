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
	std::fstream arch;
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

	void guardar_y_cerrar();
};



#endif /* SERVER_BASE_DE_DATOS_USUARIO_H_ */
