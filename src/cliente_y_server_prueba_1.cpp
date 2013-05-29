#define prueba1
#ifndef prueba1
#include <string>
#include <iostream>
#include "common_base_de_datos.h"
#include "cliente_controlador.h"
#include "server_controlador.h"

int main(int args, char**argv)
{
	/*
	// creo server
	const string dir_serv("localhost");

	const string puerto1("4000");
	const string puerto2("4001");
	string nombre_usuario("pepito");
	string contrasenia ("1234");
	string intervalo_polling("50");

	ServerControlador serv (puerto1,puerto2, dir_serv);

	// creo cliente y trato de hacer login
	ClienteControlador cli;
	string dir ("cli_prueba1");
	cli.set_directorio(dir);
	cli.login("localhost", puerto1, puerto2, nombre_usuario, contrasenia, intervalo_polling);

	// server trata de aceptar conexion



	// envio un archivo desde el cliente hacia el server
	string nombre_arch("pepito.txt");
	Modificacion mod(SUBIR_NUEVO_ARCHIVO, nombre_arch, 0,0);
	cli.aplicar_modificacion(mod);
*/
	Modificacion mod(BORRAR_ARCHIVO_LOCAL,"aux.txt",0,0);
	ClienteControlador cli;
	string dir ("/home/jorge/workspace2/tp5 tallerg/src");
	cli.set_directorio(dir);

	cli.base_de_datos.abrir(dir);
	cli.aplicar_modificacion(mod);
	cout <<"borrado"<<endl;
}














#endif


