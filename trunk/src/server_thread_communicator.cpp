#include "server_thread_communicator.h"
#include "common_hashing.h"
#include <cstring>
#include <syslog.h>

ServerCommunicator::ServerCommunicator(const string &dir, int fd1, int fd2, const string &password)
	: Controlador(dir, fd1, fd2, password), Thread(), vinculados(NULL), smpt(dir)
{
	sock1.set_password(password);
	sock2.set_password(password);
	base_de_datos.abrir(dir);
}

void ServerCommunicator::actuar_segun_modif_recibida(Modificacion &mod)
{
	Lock(*smpt.data().get_mutex(mod.nombre_archivo.c_str()));
	Lock(*smpt.data().get_mutex(mod.nombre_alt_o_hash.c_str()),mod.nombre_alt_o_hash!="");
	Lock(*smpt.data().get_mutex(NOMBRE_ARCH_IND));

	switch(mod.accion)
	{
		case NUEVO:
			if (base_de_datos.estaIndexado(mod.nombre_archivo))
			{
				sock1.enviar_flag(YA_APLICADA);
				// TODO: Comparar hashes
				syslog(LOG_WARNING, "Modificacion NUEVO: %s ya estaba aplicada.", mod.nombre_archivo.c_str());
				return;
			}
			break;
		case BORRADO:
			if (base_de_datos.estaIndexado(mod.nombre_archivo), false)
			{
				sock1.enviar_flag(YA_APLICADA);
				syslog(LOG_WARNING, "Modificacion BORRADO: %s ya estaba aplicada.", mod.nombre_archivo.c_str());
				return;
			}
			break;
		case COPIADO:
		case RENOMBRADO:
			if (base_de_datos.estaIndexado(mod.nombre_archivo)
					|| base_de_datos.estaIndexado(mod.nombre_alt_o_hash, false) ) // Ya existe un archivo con ese nombre
			{
				sock1.enviar_flag(CONFLICTO);
				syslog(LOG_WARNING, "Modificacion RENOMBRADO o COPIADO: %s - %s genero un conflicto."
						,mod.nombre_archivo.c_str(), mod.nombre_alt_o_hash.c_str());
				return;
			}
			break;
		case EDITADO:
			//base_de_datos.registrar_editado(mod.nombre_archivo);
			//TODO: Revisar el hash
			if (!base_de_datos.estaIndexado(mod.nombre_archivo))
			{
				sock1.enviar_flag(CONFLICTO);
				syslog(LOG_WARNING, "Modificacion EDICION: %s genero un conflicto.", mod.nombre_archivo.c_str());
				return;
			}
			break;
		default:
			break;
	}
	syslog(LOG_DEBUG, "Enviando flag OK");
	sock1.enviar_flag(OK);
	bool exito = false;

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
			if (exito) base_de_datos.registrar_editado(mod.nombre_archivo);
			else base_de_datos.eliminar_archivo_temporal(mod.nombre_archivo);
		}
			break;
		case RENOMBRADO:
			exito = base_de_datos.renombrar(mod.nombre_alt_o_hash, mod.nombre_archivo);
			if (exito)
				base_de_datos.registrar_renombrado(mod.nombre_archivo, mod.nombre_alt_o_hash);
			break;
		case COPIADO:
			exito = base_de_datos.copiar(mod.nombre_alt_o_hash, mod.nombre_archivo);
			if (exito)
				base_de_datos.registrar_copiado(mod.nombre_archivo, mod.nombre_alt_o_hash);
			break;
		default: // Ignoro si llega otra cosa
			break;
	}
	if (exito)
	{
		syslog(LOG_DEBUG, "Notificando otros clientes de la modificacion");
		sock1.enviar_flag(OK);
		notificar_todos(mod);
	}
	else
	{
		sock1.enviar_flag(FAIL);
		syslog(LOG_ERR, "Modificacion SIN exito, NO se notifica al resto");
	}
	syslog(LOG_DEBUG, "Fin del procesado.");
}

void ServerCommunicator::procesar_flag(PacketID flag)
{
	syslog(LOG_DEBUG, "Procesando flag");
	switch(flag)
	{
		case(MODIFICACION):
				{
				syslog(LOG_DEBUG, "Flag es MODIFICACION");
				Modificacion mod;
				sock1.recibir_modif(mod); // Recibe mod
				syslog(LOG_DEBUG, "Modificacion: %s", mod.toString().c_str());
				actuar_segun_modif_recibida(mod); // Ejecuta mod
				}
				break;
		case(PEDIDO_ARCHIVO_ENTERO):
				{
					syslog(LOG_DEBUG, "Flag es PEDIDO_ARCHIVO_ENTERO");
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
					syslog(LOG_DEBUG,"Flag es PEDIDO_ARCHIVO_EDICIONES");
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
					syslog(LOG_DEBUG,"Flag es PEDIDO_INDICE");
					Lock(*smpt.data().get_mutex(NOMBRE_ARCH_IND));
					string nomb(NOMBRE_ARCH_IND);
					ifstream archIndice;
					base_de_datos.abrir_para_leer(nomb,archIndice);
					sock1.enviar_archivo(archIndice);
					archIndice.close();
				}
				break;
		default:
				// Otros casos no deberian llegar nunca "sueltos", hubo un error
				throw std::runtime_error("Error en la conexion, se recibio un flag invalido.");
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
			syslog(LOG_DEBUG, "Esperando Flag");
			sock1.recibir_flag(flag);
			syslog(LOG_DEBUG, "Cliente envio Flag: %i", flag);
			if (flag == LOGOUT)
			{
				correr = false;
				syslog(LOG_DEBUG, "Cliente cerro sesion.");
			}else{
				procesar_flag(flag);
			}
		}
		catch (exception &e)
		{
			if(correr==true){
				syslog(LOG_ERR, "Se perdio la conexion con el cliente: %s. Estado correr: %i"
						, e.what(), correr);
			}else{
				syslog(LOG_DEBUG, "Se cierra el thread de server por: '%s' tras haber recibido senial para cerrarse",
						e.what());
			}
			correr = false;
		}
	}
	syslog(LOG_DEBUG, "ServerComunicator: Termino ejecucion");
}

void ServerCommunicator::setVinculados(list<ServerCommunicator*> *vinc)
{
	vinculados = vinc;
}

void ServerCommunicator::notificar_todos(Modificacion &mod)
{
	for (list<ServerCommunicator*>::iterator it = vinculados->begin(); it != vinculados->end(); ++it)
	{
		// Transmito la modificacion a todos los vinculados que aun esten corriendo
		// Si no esta corriendo, el servidor tiene aun que limpiarlo, por ahora lo ignoro
		if (*it != this && (*it)->correr) (*it)->propagar_cambio(mod);
	}
}
void ServerCommunicator::stop(){
	syslog(LOG_DEBUG, "Server comunicator: Se pidio que se detenga");
	this->correr = false;
	sock1.cerrar();
	sock2.cerrar();
}
void ServerCommunicator::propagar_cambio(Modificacion &mod)
{
	//Lock temp (mutex);
	sock2.enviar_modif(mod);
}
ServerCommunicator::~ServerCommunicator(){
	this->stop();
}
