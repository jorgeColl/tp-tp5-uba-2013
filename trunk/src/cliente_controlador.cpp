#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include "cliente_controlador.h"
using namespace std;

ClienteControlador::ClienteControlador() : terminar(false),delay_polling(POLLING_DEFAULT) {}

void ClienteControlador::login(string server, string puerto1, string puerto2,
		string usuario, string contrasenia, string polling)
{
	bool exito;
	//Estos tiran excepciones dependiendo que falle
	sock1.conectar(server.c_str(), puerto1.c_str());

	string mensaje;
	sock1.enviar_flag(LOGIN);
	sock1.enviar_msg_c_prefijo(usuario, BYTES_CLI_CONT);
	sock1.enviar_msg_c_prefijo(contrasenia, BYTES_CLI_CONT);
	exito = sock1.enviarLen(mensaje.c_str(), mensaje.size());
	if(!exito) throw runtime_error("Fallo en la comunicación con el servidor.");

	PacketID login;
	exito = sock1.recibir_flag(login);
	if(!exito) throw runtime_error("Fallo en la comunicación con el servidor.");
	if(login != OK) throw runtime_error("Los datos de login son incorrectos.");

	sock2.conectar(server.c_str(),puerto2.c_str());
}

vector<Modificacion> ClienteControlador::pedir_y_comparar_indices()
{
	sock1.enviar_flag(PEDIDO_INDICE);
	fstream idxServ;
	base_de_datos.abrir_para_escribir(NOM_IDX_SERV, idxServ);
	sock1.recibir_archivo(idxServ);
	base_de_datos.abrir_para_leer(NOM_IDX_SERV, idxServ);
	vector<Modificacion> modifs = base_de_datos.comparar_indices(idxServ);
	idxServ.close();
	return modifs;
}

vector<Modificacion> ClienteControlador::recibir_modificaciones() {
	vector<Modificacion> aux;
	return aux;
}

void ClienteControlador::start()
{
	//Aplicacion inicial
	bool exito;
	vector<Modificacion> mod1 = pedir_y_comparar_indices();
	for(size_t i=0; i<mod1.size(); ++i)
	{
		//mod1[i].efectuar_cambios(*this);
		exito = aplicar_modificacion(mod1[i]);
	}

	while(terminar != true)
	{
		vector<Modificacion> mod2 = comprobar_cambios_locales();
		for(size_t i=0;i<mod2.size();++i)
		{
			//mod2[i].efectuar_cambios(*this);
			exito = aplicar_modificacion(mod2[i]);
			if(!exito) return;
		}
		vector<Modificacion> mod3 = recibir_modificaciones();
		for(size_t i=0;i<mod3.size();++i)
		{
			//mod3[i].efectuar_cambios(*this);
			exito = aplicar_modificacion(mod3[i]);
			if(!exito) return;
		}
		sleep(delay_polling);
	}
	return;
}

