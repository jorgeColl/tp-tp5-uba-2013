#ifndef COMMON_HASHING_H_
#define COMMON_HASHING_H_

#include <fstream>
using namespace std;

/**
 * @file common_hashing.h
 * @brief Funciones de hashing para distintos casos
 */

/**
 * @brief Devuelve el hash MD5 de un archivo entero
 * @param path_arch path al archivo que se leera
 * @param hash String donde se guardara el hash
 * @return True si el proceso fue exitoso, false de no serlo
 */
bool MD5_arch(const string &path_arch, string &hash);

/**
 * @brief Devuelve el hash MD5 de un archivo entero
 * @param path_arch path al archivo que se leera
 * @return String donde se guardara el hash
 * @throws hlException en caso de fallar
 */
string MD5_arch(const string &path_arch);

/**
 * @brief Devuelve el hash MD5 aplicado a un "bloque" particular del archivo
 * @param arch Archivo que se leera
 * @param offset Offset a utilizar antes de empezar a leer
 * @param tamBloq Longitud de los bloques a utilizar para hacer el calculo
 * @param hash String donde se guardara el hash
 * @return True si el proceso fue exitoso, false de no serlo
 */
bool MD5_bloque(istream &arch, off_t offset, off_t tamBloq, string &hash);
/**
 * @brief devuelve el hash de un string pasado por parametro
 */
string MD5_string(const string& mensaje);

#endif /* COMMON_HASHING_H_ */
