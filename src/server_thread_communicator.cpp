#include "server_thread_communicator.h"
#include "common_hashing.h"
#include <cstring>

ServerCommunicator::ServerCommunicator(const string &dir, int fd1, int fd2, const string &password)
	: Controlador(dir, fd1, fd2, password), Thread(), vinculados(NULL), smpt(dir)
{
	base_de_datos.abrir(dir);
}

void ServerCommunicator::actuar_segun_modif_recibida(Modificacion &mod)
{
	switch(mod.accion)
		{
			case NUEVO:
				if (base_de_datos.estaIndexado(mod.nombre_archivo)){
					sock1.enviar_flag(YA_APLICADA);
				return;
				}
				break;
			case BORRADO:
				if (!base_de_datos.estaIndexado(mod.nombre_archivo)){
					sock1.enviar_flag(YA_APLICADA);
				return;
				}
				break;
			default:break;
		}
	// TODO: Fijarse si ya esta aplicada/es vieja y emitir YA_ESTA si es el caso
	cout<<"Eenviando flag OK"<<endl;
	sock1.enviar_flag(OK);
	cout<<"Flag enviado :)"<<endl;
	bool exito = false;
	cout<<"antes lock"<<endl;

	Lock(*smpt.data().get_mutex(mod.nombre_archivo.c_str()));
	Lock* lock = 0;
	if(mod.nombre_archivo_alt =="")
	{
		lock = new Lock(*smpt.data().get_mutex(mod.nombre_archivo_alt.c_str()));
	}
	Lock(*smpt.data().get_mutex(NOMBRE_ARCH_IND));

	cout<<"despues lock"<<endl;
	switch(mod.accion)
	{
		case NUEVO:
			{
				ofstream destino;
				exito = base_de_datos.abrir_para_escribir(mod.nombre_archivo, destino);
				if(exito)
				{
					sock1.recibir_archivo(destino);
					destino.close();
					base_de_datos.registrar_nuevo(mod.nombre_archivo);
				}
			}
			break;
		case BORRADO:
			exito = base_de_datos.eliminar_archivo(mod.nombre_archivo);
			if (exito) base_de_datos.registrar_eliminado(mod.nombre_archivo);
			break;
		case EDITADO:
		{
			ofstream destino;
			base_de_datos.abrir_para_escribir_temporal(mod.nombre_archivo, destino);
			ifstream original;
			base_de_datos.abrir_para_leer(mod.nombre_archivo, original);
			sock1.recibir_edicion(original, destino);
			// Terminamos
			original.close();
			destino.close();
			exito = base_de_datos.renombrar_temporal(mod.nombre_archivo);
			if (exito) base_de_datos.registrar_modificado(mod.nombre_archivo);
			else base_de_datos.eliminar_archivo_temporal(mod.nombre_archivo);
		}
			break;
		case RENOMBRADO:
			exito = base_de_datos.renombrar(mod.nombre_archivo_alt, mod.nombre_archivo);
			if (exito)
			{
				base_de_datos.registrar_renombrado(mod.nombre_archivo, mod.nombre_archivo_alt);
			}
			break;
		case COPIADO:
			exito = base_de_datos.copiar(mod.nombre_archivo_alt, mod.nombre_archivo);
			if (exito)
			{
				base_de_datos.registrar_copiado(mod.nombre_archivo, mod.nombre_archivo_alt);
			}
			break;
		default: // Igstnoro si llega otra cosa
			break;
	}
	if (exito)
	{
		cout<<"Notificando otros clientes de la modificacion"<<endl;
		sock1.enviar_flag(OK);
		notificar_todos(mod);
	}
	else
	{
		sock1.enviar_flag(FAIL);
		cout<<"Modificacion SIN exito, NO se notifica al resto"<<endl;
	}
	cout << "Fin del procesado." << endl;
	if(mod.nombre_archivo_alt =="")
	{
		delete lock;
	}
}

void ServerCommunicator::procesar_flag(PacketID flag)
{
	cout<<"procesando flag"<<endl;
	switch(flag)
	{
		case(LOGOUT):
				// cerrar coneccion y salir
				break;
		case(MODIFICACION):
				{
				cout<<"Flag es MODIFICACION"<<endl;
				Modificacion mod;
				sock1.recibir_modif(mod); // Recibe mod
				cout << mod << endl;
				actuar_segun_modif_recibida(mod); // Ejecuta mod
				}
				break;
		case(PEDIDO_ARCHIVO_ENTERO):
				{
					cout<<"Flag es PEDIDO_ARCHIVO_ENTERO"<<endl;
					string nombre;
					sock1.recibir_msg_c_prefijo(nombre, BYTES_PREF_NOMBRE);
					Lock(*smpt.data().get_mutex(nombre.c_str()));
					ifstream arch;
					base_de_datos.abrir_para_leer(nombre, arch);
					sock1.enviar_archivo(arch);
					arch.close();
				}
				break;
		case(PEDIDO_ARCHIVO_EDICIONES):
				{
					cout<<"Flag es PEDIDO_ARCHIVO_EDICIONES"<<endl;
					string nombre;
					sock1.recibir_msg_c_prefijo(nombre, BYTES_PREF_NOMBRE);
					Lock(*smpt.data().get_mutex(nombre.c_str()));
					ifstream arch;
					base_de_datos.abrir_para_leer(nombre, arch);
					sock1.enviar_edicion(arch);
					arch.close();
				}
				break;
		case(PEDIDO_INDICE):
				// Devuelve el indice
				{
					cout<<"Flag es PEDIDO_INDICE"<<endl;
					string nomb(NOMBRE_ARCH_IND);
					ifstream archIndice;
					//Lock(this->mutex_ind);
					base_de_datos.abrir_para_leer(nomb,archIndice);
					sock1.enviar_archivo(archIndice);
					archIndice.close();
				}
				break;
		default:
				cout<<"ALERTA !!! flag no identificado recibido !!!!!"<<endl;
				// Otros casos no deberian llegar nunca "sueltos", se ignoran
				break;
	}
}

void ServerCommunicator::ejecutar()
{
	// Se comunica con el cliente
	PacketID flag = ZERO;
	while(correr)
	{
		try
		{
			sock1.recibir_flag(flag);
			cout<<"cliente envio mensaje"<<endl;
			if (flag == LOGOUT)
			{
				correr = false;
				cout << "Cliente cerro sesion." << endl;
			}
			else procesar_flag(flag);
		}
		catch (exception &e)
		{
			correr = false;
			cout << "Se perdio la conexion con el cliente:" << e.what() << endl;
		}
	}
}

void ServerCommunicator::setVinculados(list<ServerCommunicator*> *vinc)
{
	vinculados = vinc;
}

void ServerCommunicator::notificar_todos(Modificacion &mod)
{
	for (list<ServerCommunicator*>::iterator it = vinculados->begin(); it != vinculados->end(); ++it)
	{
		if (*it != this) (*it)->propagar_cambio(mod);
	}
}

void ServerCommunicator::propagar_cambio(Modificacion &mod)
{
	//Lock temp (mutex);
	sock2.enviar_modif(mod);
}
