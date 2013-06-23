#ifndef COMMON_BASE_DE_DATOS_USUARIO_H_
#define COMMON_BASE_DE_DATOS_USUARIO_H_

#include <fstream>
#include <string>
#include <map>
#include <stdexcept> 	// Excepciones genericas
#include <cerrno>
#include <cstring>
#include <string>
#include "defines.h"

using namespace std;

/**
 * @class BaseDeDatosUsuario common_base_de_datos_usuario.h "common_base_de_datos_usuario.h"
 * @brief Maneja los pares usuario:password para logearse y demas
 * @details En principio solo es un hash en ram
 */
class BaseDeDatosUsuario
{
private:
	string path;
	std::map<std::string ,std::string> usu_contr;
public:
	/**
	 * @brief Constructor
	 * @param path Path al archivo a usarse
	 */
	BaseDeDatosUsuario(const string &path);
	/**@brief cambia el archivo a abrir*/
	void setPath(const string &path);
	/**@brief abre y carga en memoria la base de datos de usuarios y su respectivas contraseña */
	void abrir();
	/**@brief dado un nombre de usuario devuelve la contraseña de este*/
	std::string get_pass(std::string& usuario);
	/**@brief Verifica si el usuario y contraseña ingresados pertenecen a un usuario de la base de datos
	 * @details la contraseña es el hash md5 de el nombre de usuario apendeado con la contraseña */
	bool usuario_contrasenia_correcto(string& usu, char* contrasenia);
	/**@brief Agrega un usuario a la base de datos */
	void agregar_usuario(const char* usuario, const char* contrasenia);
	/**@brief Elimina un usuari de la base de datos */
	void eliminar_usuario(const char* usuario);
	/**@brief guarda los datos que estan en la ram en disco, si no se quiere guardar a disco
	 * simplemente no se hace nada y se elimina comunmente como cualquier instancia*/
	void guardar_a_disco();
	/**@brief POSIBLE FUNCION a implementar en el futuro, lee otra vez del archivo y agrega
	 * los nuevos usuarios que encuentre en el archivo */
	void actualizar();
};



#endif /* COMMON_BASE_DE_DATOS_USUARIO_H_ */
