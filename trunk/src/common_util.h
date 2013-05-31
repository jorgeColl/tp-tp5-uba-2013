#ifndef COMMON_UTIL_H_
#define COMMON_UTIL_H_

#include <string>
using namespace std;

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


#endif /* COMMON_UTIL_H_ */
