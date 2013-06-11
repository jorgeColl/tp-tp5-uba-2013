#ifndef DEFINES_H_
#define DEFINES_H_

#define DB_USU_ARCH "usu.db"
#define MONIT_GLADE "server.glade"
#define ARCH_PREFS_SERV "server.cfg"
#define PUERTO1_DEF "12700"
#define PUERTO2_DEF "12701"
#define DIR_DEF_SERV "./serverDir"

#define NOMBRE_ARCH_IND ".auindice"
#define EXT_TMP ".tmp"
#define BYTES_HASH 16
//Definicion de bytes para prefijo de largo de nombre de archivo
#define BYTES_PREF_NOMBRE 1
#if NAME_MAX > 255 //Si el sistema operativo permite nombres mas largos le doy 2 bytes
#undef BYTES_PREF_NOMBRE
#define BYTES_PREF_NOMBRE 2
#endif

#endif /* DEFINES_H_ */
