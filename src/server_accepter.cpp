#include "server_accepter.h"
#include "common_util.h"
#include <sys/stat.h>	//mkdir
#include "defines.h"

Accepter::Accepter(const string &dir, const string &puerto1, const string &puerto2) :
		base_datos_usu(unirPath(dir,db_usu_arch)), dir(dir),
		puerto1(puerto1), puerto2(puerto2), comunicadores() {}

Accepter::~Accepter()
{
	sock_prot1.cerrar();
	sock_prot2.cerrar();
}

void Accepter::ejecutar()
{
	correr = true;
	base_datos_usu.abrir();
	sock_prot1.escuchar(puerto1.c_str(), MAX_COLA);
	sock_prot2.escuchar(puerto2.c_str(), MAX_COLA);
	while (correr)
	{
		bool exito = aceptar_conexion();
		if (exito) cout << "Conexion exitosa desde un cliente." << endl;
		else cout << "Aceptacion de cliente fallida." << endl;
	}
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
	sock_prot1.cerrar();
	sock_prot2.cerrar();
	// JOIN DE los ServerCommunicator
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
			(usuario.c_str(), contrasenia.c_str());
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
				(unirPath(dir, usuario).c_str(), fd_nuevo_1, fd_nuevo_2);
		comunicadores[usuario].push_back(comu);
		comu->setVinculados(&comunicadores[usuario]);
		comu->start();
	}
	// Nota: El delete de sock1 no llama a close, asi que con pasar su file descriptor esta bien
	return true;
}
