#include "server_accepter.h"
#include "common_util.h"
#include <sys/stat.h>	//mkdir
#include "defines.h"
#include "common_observador_recibido.h"

Accepter::Accepter(const string &dir, const string &puerto1, const string &puerto2) :
		base_datos_usu(unirPath(dir,DB_USU_ARCH)), dir(dir),
		puerto1(puerto1), puerto2(puerto2), comunicadores() {}

Accepter::~Accepter()
{
	sock_prot1.cerrar();
	sock_prot2.cerrar();
}
void Accepter::limpiar_procesos_terminados()
{
	vector<string>a_eliminar_en_map;
	map<string, list<ServerCommunicator*> >::iterator it;
	for(it = comunicadores.begin(); it!= comunicadores.end();it++){
		vector<ServerCommunicator*> a_eliminar;
		list<ServerCommunicator*>::iterator seg_it;
		for(seg_it = it->second.begin(); seg_it!= it->second.end(); seg_it++){
			if ((*seg_it)->correr == false){
				a_eliminar.push_back((*seg_it));
			}
		}
		for (size_t i=0;i<a_eliminar.size();++i) {
			list<ServerCommunicator*>::iterator ter_it;
			it->second.remove(a_eliminar[i]);
			delete(a_eliminar[i]);
		}
		if(it->second.size() == 0){
			a_eliminar_en_map.push_back(it->first);
		}
	}
	for(size_t j=0;j<a_eliminar_en_map.size();++j){
		comunicadores.erase(a_eliminar_en_map[j]);
	}

}
void Accepter::ejecutar()
{
	correr = true;
	base_datos_usu.abrir();
	sock_prot1.escuchar(puerto1.c_str(), MAX_COLA);
	sock_prot2.escuchar(puerto2.c_str(), MAX_COLA);
	size_t contador = 0;
	ObserbadorRecibido obs;
	obs.start();
	while (correr)
	{
		bool exito = aceptar_conexion();
		if (exito){
			contador++;
			if(contador >= CONEXCIONES_ACEPTADAS_PARA_BORRAR_MUERTOS){
				limpiar_procesos_terminados();
				cout << "se realizo borrado de muertos"<<endl;
			}
			cout << "Conexion exitosa desde un cliente." << endl;
		}
		else cout << "Aceptacion de cliente fallida." << endl;
	}
	cout<<"Accepter termina ejecucion"<<endl;
	obs.stop();
	obs.join();
}

void Accepter::stop()
{
	Thread::stop();
	Lock lock_temp (mutex); // Para que esta este mutex aca? Si se llamara a stop desde distintos lugares no pasa nada
	for (map<string, list<ServerCommunicator*> >::iterator it = comunicadores.begin(); it != comunicadores.end(); ++it)
	{
		for (list<ServerCommunicator*>::iterator itL = it->second.begin(); itL != it->second.end(); ++itL)
		{
			(*itL)->stop();
		}
	}
	cout<<"cerrando conexiones"<<endl;
	sock_prot1.cerrar();
	sock_prot2.cerrar();
	// JOIN DE los ServerCommunicator
	for (map<string, list<ServerCommunicator*> >::iterator it = comunicadores.begin(); it != comunicadores.end(); ++it) {
		for (list<ServerCommunicator*>::iterator itL = it->second.begin(); itL != it->second.end(); ++itL) {
			(*itL)->stop();
			(*itL)->join();
			delete (*itL);
		}
	}
	cout<<"conexiones cerradas"<<endl;

}

bool Accepter::aceptar_conexion()
{
	cout << "Aguardando conexion." << endl;
	int fd_nuevo_1 = sock_prot1.aceptar();
	if (fd_nuevo_1 < 0) return false;
	cout << "Conexion recibida, esperando login." << endl;
	// Recibo mensaje
	SocketProt sock1(fd_nuevo_1);
	PacketID login = FAIL;
	sock1.recibir_flag(login);
	if (login != LOGIN) return false; // Veo que sea el paquete correcto
	cout << "Flag de LOGIN recibido." << endl;
	string usuario;
	string contrasenia;
	sock1.recibir_msg_c_prefijo(usuario, BYTES_USER_PASS);
	sock1.recibir_msg_c_prefijo(contrasenia, BYTES_USER_PASS);
	bool login_correcto = base_datos_usu.usuario_contrasenia_correcto
			(usuario, contrasenia);
	if (!login_correcto)
	{
		cout << "Password incorrecta." << endl;
		sock1.enviar_flag(FAIL);
		sock1.cerrar();
		return false;
	}
	else
	{
		cout << "Password correcta." << endl;
		// Si la carpeta del usuario no existe la creo, sino esta llamada no hace nada
		mkdir(unirPath(dir,usuario).c_str(), 0700);
		// Si mensaje bueno creo el otro socket
		sock1.enviar_flag(OK);
		int fd_nuevo_2 = sock_prot2.aceptar();
		if(fd_nuevo_2 < 0)
		{
			sock1.cerrar();
			return false;
		}
		ServerCommunicator* comu = new ServerCommunicator
				(unirPath(dir, usuario), fd_nuevo_1, fd_nuevo_2, contrasenia);
		comunicadores[usuario].push_back(comu);
		comu->setVinculados(&comunicadores[usuario]);
		comu->start();
	}
	// Nota: El delete de sock1 no llama a close, asi que con pasar su file descriptor esta bien
	return true;
}
