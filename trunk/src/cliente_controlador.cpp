#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>	// Excepciones estandar
#include "cliente_controlador.h"
using namespace std;

ClienteControlador::ClienteControlador() : Controlador(),delay_polling(POLLING_DEFAULT),
		notificador(this) {}

void ClienteControlador::login(string server, string puerto1, string puerto2,
		string usuario, string contrasenia, string polling)
{
	if (conectado) throw runtime_error("El cliente ya esta corriendo."); // Nunca deberia darse
	try
	{
		base_de_datos.cerrar();
		sock1.cerrar();
		sock2.cerrar();
		delay_polling = atoi(polling.c_str());
		base_de_datos.abrir(dir);
		cout << "Intentado conectarse" << endl;
		//Estos tiran excepciones dependiendo que falle
		sock1.conectar(server.c_str(), puerto1.c_str());
		sock1.enviar_flag(LOGIN);
		sock1.enviar_msg_c_prefijo(usuario, BYTES_USER_PASS);
		sock1.enviar_msg_c_prefijo(contrasenia, BYTES_USER_PASS);
		PacketID login;
		sock1.recibir_flag(login);
		if(login != OK) throw runtime_error("Los datos de login son incorrectos.");
		notificador.conectar(server.c_str(),puerto2.c_str());
		conectado = true;
		cout << "Conexion exitosa." << endl;
	}
	catch(runtime_error &e) //Cierro los sockets y propago
	{
		base_de_datos.cerrar();
		sock1.cerrar();
		sock2.cerrar();
		throw e;
	}
}

list<Modificacion> ClienteControlador::pedir_y_comparar_indices()
{
	sock1.enviar_flag(PEDIDO_INDICE);
	// Lo recibo en ram pues lo voy a desechar tras comparar y es chico
	stringstream idxServ;
	sock1.recibir_archivo(idxServ);
	list<Modificacion> modifs = base_de_datos.comparar_indices(idxServ);
	return modifs;
}

void ClienteControlador::AplicarNotificacion(Modificacion &modif)
{
	Lock aplicandoCambio(mutexCambios);
	cout << "Se recibio modificacion: " << modif << endl;
	bool exito = aplicar_modificacion(modif);
	// Si falla, solo logeo, se supone que el indice fisico y demas quedo en condiciones
	if (!exito) cout << "Error al aplicar una modificacion recibida." << endl;
}

void ClienteControlador::ejecutar()
{
	bool exito;
	correr = true;
	//Comentado temporalmente para testear hasta que este terminado
	cout << "Ejecutando sincronizacion inicial" << endl;
	// Comprobacion inicial, comentada por ahora
	/*list<Modificacion> modServer = pedir_y_comparar_indices();
	for (list<Modificacion>::iterator it = modServer.begin(); it != modServer.end(); ++it)
	{
		aplicar_modificacion(*it);
	}*/
	notificador.start(); // Recien aca pongo a correr el notificador, no deberia haber "perdida de datos"
	while(correr && conectado)
	{
		cout << "Esperando los " << delay_polling << " segundos de polling." << endl;
		sleep(delay_polling);
		Lock cambiosLocales(mutexCambios);
		cout << "Ejecutando polling" << endl;
		list<Modificacion> modLocales = comprobar_cambios_locales();
		cout << "Numero de cambios locales encontrados: " << modLocales.size() << endl;
		for (list<Modificacion>::iterator it = modLocales.begin(); it != modLocales.end(); ++it)
		{
			cout << *it << endl;
			exito = aplicar_modificacion(*it);
			// Si falla, solo logeo, se supone que el indice fisico y demas quedo en condiciones
			if (!exito) cout << "Error al aplicar una modificacion local." << endl;
		}
	}
	sock1.enviar_flag(LOGOUT);
	/*sock1.cerrar();
	notificador.stop();
	notificador.join();*/
	return;
}

