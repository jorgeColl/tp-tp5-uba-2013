#define prueba1
#ifndef prueba1
#include "cliente_controlador.h"
#include "server_controlador.h"
int main(int args, char**argv)
{
	// creo server
	string dir_serv("serv_prueba1");
	string puerto("8080");
	ServerControlador serv(puerto, dir_serv);

	// creo cliente
	ClienteControlador cli;
	string dir ("cli_prueba1");
	cli.set_directorio(dir);

	// envio un archivo desde el cliente hacia el server
	string nombre_arch("pepito.txt");
	Modificacion mod(SUBIR_NUEVO_ARCHIVO, nombre_arch, 0,0);
	cli.aplicar_modificacion(mod);



}














#endif


