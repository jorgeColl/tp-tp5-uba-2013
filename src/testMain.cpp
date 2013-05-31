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
		cout << "Abierto: " << db.abrir("./testDir") << endl;
		vector<Modificacion> modifs = db.comprobar_cambios_locales();
		for (vector<Modificacion>::iterator it = modifs.begin(); it != modifs.end(); ++it)
		{
			cout << *it << endl;
			switch(it->accion)
			{
				case SUBIR_NUEVO_ARCHIVO:
					cout << "intento registrar: " << db.registrar_nuevo(it->nombre_archivo) << endl;
					break;
				case MANDAR_A_BORRAR_ARCHIVO:
					break;
				case SUBIR_MOD_ARCHIVO:
					break;
				case MANDAR_COPIA_ARCHIVO:
					break;
				case MANDAR_RENOMBRE_ARCHIVO:
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
