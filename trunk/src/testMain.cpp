#include <cstdlib>
#include <sys/stat.h>
#include <iostream>
#include "common_base_de_datos.h"

using namespace std;

int main(int argc, char** argv)
{
	try
	{
		umask(0077); // Se hace &~umask al calcular permisos en el programa
		// Test de solo la base de datos, hace falta poner y borrar archivos a mano
		//cout << "Borrar dir: " << system("rm -R ./testDir") << endl;
		//cout << "Crear dir: " << mkdir("./testDir",0777) << endl; //Permisos depende de umask
		BaseDeDatos db;
		db.abrir("./testDir");
		list<Modificacion> modifs = db.comprobar_cambios_locales();
		for (list<Modificacion>::iterator it = modifs.begin(); it != modifs.end(); ++it)
		{
			cout << *it << endl;
			switch(it->accion)
			{
				case SUBIR_NUEVO_ARCHIVO:
					db.registrar_nuevo(it->nombre_archivo);
					break;
				case MANDAR_A_BORRAR_ARCHIVO:
					db.registrar_eliminado(it->nombre_archivo);
					break;
				case SUBIR_MOD_ARCHIVO:
					db.registrar_modificado(it->nombre_archivo);
					break;
				case MANDAR_COPIA_ARCHIVO:
					db.registrar_copiado(it->nombre_archivo, it->nombre_archivo_alt);
					break;
				case MANDAR_RENOMBRE_ARCHIVO:
					db.registrar_renombrado(it->nombre_archivo, it->nombre_archivo_alt);
					break;
				default:
					cout << "Error" << endl;
					break;
			}
		}

		//cout << "Borrar dir: " << system("rm -R ./testDir/")  << endl;
		return 0;
	}
	catch (exception &e)
	{
		cout << "Error: " << e.what() << endl;
	}
	return 1;
}
