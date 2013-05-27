#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "cliente_controlador.h"

using namespace std;
ClienteControlador::ClienteControlador(){
	terminar = false;
	dir = "default";

}
bool ClienteControlador::login(std::string server, std::string puerto1, std::string puerto2, std::string usuario, std::string contrasenia)
{
	cout<<"clienteControlador: me hicieron login con:"<<endl;
	cout<<"usuario:"<<usuario<<" contreseña:"<<contrasenia<<endl;
	bool exito;
	//Estos tiran excepciones dependiendo que falle
	sock1.conectar(server.c_str(),puerto1.c_str());
	sock2.conectar(server.c_str(),puerto2.c_str());

	string mensaje;
	mensaje += "1 ";
	mensaje += usuario;
	mensaje += contrasenia;
	exito = sock1.enviarLen(mensaje.c_str(), mensaje.size());
	if(!exito) {return false;}

	Packet login;
	exito = sock1.recibir_flag(login);
	if(!exito) {return false;}
	if(login == LOGIN_OK)
	{
		return true;
	}
	return false;
}
void ClienteControlador::set_directorio(std::string dir){
	this->dir = dir;
}
bool ClienteControlador::armar_indice_local() {
	return base_de_datos.abrir(dir);
}
vector<Modificacion> ClienteControlador::pedir_y_comparar_indices() {
	// mensaje al server pidiendo el indice suyo
	//recibir_indice();
	// realiza la comparacion entre el indice local y el recibido por el server
	return base_de_datos.comparar_indices();
}
vector<Modificacion> ClienteControlador::recibir_modificaciones() {
	vector<Modificacion> aux;
	return aux;
}
bool ClienteControlador::borrar_archivo(std::string& nombre_archivo){
	return base_de_datos.eliminar_archivo(nombre_archivo);
}
bool ClienteControlador::mandar_a_borrar (Modificacion& mod) {
	/* cambio el "punto de vista" de la modificacion para que cuando el
	server lea esto directamente borre el archivo que tiene en su directorio*/
	mod.accion=BORRAR_ARCHIVO_LOCAL;

	return sock1.enviar_modif(mod);
}
bool ClienteControlador::pedir_nuevo_archivo(Modificacion& mod){
	// manda mensaje a server pidiendo que transmita archivo
	// lo guarda temporalmente en un archivo
	// le dice a base de datos que guarde al nuevo archivo

	// cambia el "punto de vista"
	mod.accion =  SUBIR_NUEVO_ARCHIVO;
	sock1.enviar_modif(mod);
	// parte prototipo
	ofstream& fd = base_de_datos.generar_archivo_temp(mod.nombre_archivo);
	sock1.recibir_archivo(fd);
	base_de_datos.cerrar_exitosamente_archivo(mod.nombre_archivo,fd);
	return true;
}
bool ClienteControlador::enviar_nuevo_archivo(std::string& nombre_archivo){
	// asumo que el archivo entra en ram (en un string)
	// pide a base de datos el archivo en string
	// pasa el archivo (que está en forma de string) al server
	sock1.enviar_flag(ARCH_NUEVO);
	// ACA LE TENDRIA QUE PEDIR A LA BASE DE DATOS POR METODO QUE ABRA EL ARCHIVO
	//ifstream fd(nombre_archivo.c_str(),ifstream::binary);
	ifstream& fd = base_de_datos.abrir_para_leer(nombre_archivo);
	sock1.enviar_archivo(fd);
	fd.close();
	delete &fd;
	return true;
}
bool ClienteControlador::modificar_archivo(std::string& nombre_archivo){
	// le indica a la base de datos que modifique el archivo
	// falta definir parametros de modificacion
	return true;
}
bool ClienteControlador::enviar_modificacion(Modificacion& mod){
	// le manda un mensaje al server indicando que un archivo se modificó y le pasa los datos
	// FALTA DEFINIR QUE DATOS PASA:
	// OP1: PASA NUMERO INI, NUMERO FIN  Y PASA BLOQUE DE ARCHIVO  MODIFICADO
	// OP2: PASA NUMERO INI, NUMERO FIN Y DESP ESPERA A QUE LE PIDAN CIERTO BOLQUE/S
	return true;
}
bool ClienteControlador::pedir_modificacion(std::string& nombre_archivo){
	// falta definir
	return true;
}
bool ClienteControlador::aplicar_modificacion(Modificacion& mod) {
	switch (mod.accion) {
	case SUBIR_NUEVO_ARCHIVO: {
		return enviar_nuevo_archivo(mod.nombre_archivo);
		break;
	}
	case BAJAR_NUEVO_ARCHIVO: {
		return pedir_nuevo_archivo(mod);
		break;
	}
	case BORRAR_ARCHIVO_LOCAL: {
		return borrar_archivo(mod.nombre_archivo);
		break;
	}
	case MANDAR_A_BORRAR_ARCHIVO: {
		return mandar_a_borrar(mod);
		break;
	}
	case SUBIR_MOD_ARCHIVO: {
		return enviar_modificacion(mod);
		break;
	}
	case BAJAR_MOD_ARCHIVO: {
		return pedir_modificacion(mod.nombre_archivo);
		break;
	}
	}
	return false;
}

bool ClienteControlador::start() {
	bool exito = armar_indice_local();
	if(!exito){return false;}

	vector<Modificacion> mod1 = pedir_y_comparar_indices();
	for(size_t i=0; i<mod1.size(); ++i){
		//mod1[i].efectuar_cambios(*this);
		exito = aplicar_modificacion(mod1[i]);
	}
	while(terminar != true) {
		vector<Modificacion> mod2 = comprobar_cambios_locales();
		for(size_t i=0;i<mod2.size();++i){
			//mod2[i].efectuar_cambios(*this);
			exito = aplicar_modificacion(mod2[i]);
			if(!exito){return false;}
		}
		vector<Modificacion> mod3 = recibir_modificaciones();
		for(size_t i=0;i<mod3.size();++i){
			//mod3[i].efectuar_cambios(*this);
			exito = aplicar_modificacion(mod3[i]);
			if(!exito){return false;}
		}

		sleep(3);
	}
	return true;

}
std::vector<Modificacion> ClienteControlador::comprobar_cambios_locales(){
	return base_de_datos.comprobar_cambios_locales();
}


