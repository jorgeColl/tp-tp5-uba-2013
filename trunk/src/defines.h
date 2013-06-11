#ifndef DEFINES_H_
#define DEFINES_H_

#include <linux/limits.h>	// NAME_MAX

// Nombres "por defecto" de archivos y de configuraciones iniciales
#define DB_USU_ARCH "usu.db"
#define MONIT_GLADE "server.glade"
#define ARCH_PREFS_SERV "server.cfg"
#define PUERTO1_DEF "12700"
#define PUERTO2_DEF "12701"
#define DIR_DEF_SERV "./serverDir"

#define NOMBRE_ARCH_IND ".auindice"
#define EXT_TMP "~"
#define BYTES_HASH 16 // Cuantos bytes ocupa un hash
//Definicion de bytes para prefijo de largo de nombre de archivo
#define BYTES_PREF_NOMBRE 1
#if NAME_MAX > 255 //Si el sistema operativo permite nombres mas largos le doy 2 bytes
#undef BYTES_PREF_NOMBRE
#define BYTES_PREF_NOMBRE 2
#endif
#define BYTES_USER_PASS 1 // Tam maximo en byes para la longitud de usuario o password

#define TAM_BLOQ 64 // Bloques de 4KB, tamanio razonable para testear.

#endif /* DEFINES_H_ */
