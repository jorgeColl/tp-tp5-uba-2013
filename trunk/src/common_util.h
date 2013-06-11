#ifndef COMMON_UTIL_H_
#define COMMON_UTIL_H_

#include <string>
using namespace std;


/**
 * @file common_util.h
 * @brief Funciones utilitarias varias que no merecen un archivo particular
 */

/**
 * @brief Se fija si un archivo existe o no
 * @param path Path al archivo
 * @return True si existe
 */
bool esArchivo(const string &path);

/**
 * @brief Se fija si un archivo existe o no
 * @param dir Directorio del archivo
 * @param nombre Nombre del archivo
 * @return True si existe
 */
bool esArchivo(const string &dir, const string &nombre);

/**
 * @brief Se fija si un nombre de archivo contiene caracteres especiales que lo hacen no apto para indexarse
 */
bool esIgnorable(const string &nombre);

/**
 * @brief Crea un path completo uniendo un path existente y un nombre de archivo
 * @param dir String con el path al directorio
 * @param nombre Nombre del archivo
 * @return String resultante
 */
string unirPath(const string dir, const string nombre);

#endif /* COMMON_UTIL_H_ */
