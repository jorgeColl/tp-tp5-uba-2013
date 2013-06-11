#include "server_thread_communicator.h"

ServerCommunicator::ServerCommunicator(const string &dir, int fd1, int fd2, const string &password)
	: Controlador(dir, fd1, fd2, password), Thread(), vinculados(NULL), base_de_datos(dir)
{
	base_de_datos.abrir(dir);
}

void ServerCommunicator::actuar_segun_modif_recibida(Modificacion &mod)
{
	// TODO: Fijarse si ya esta aplicada/es vieja y emitir YA_ESTA si es el caso
	sock1.enviar_flag(OK);
	bool exito = false;
	switch(mod.accion)
	{
		case NUEVO:
			{
			ofstream destino;
			exito = base_de_datos.abrir_para_escribir(mod.nombre_archivo, destino);
			sock1.recibir_archivo(destino);
			base_de_datos.cerrar_archivo(mod.nombre_archivo, destino);
			base_de_datos.registrar_nuevo(mod.nombre_archivo);
			sock1.enviar_flag(OK);
			}
			break;
		case BORRADO:
			exito = base_de_datos.eliminar_archivo(mod.nombre_archivo);
			if (exito) { sock1.enviar_flag(OK); }
			else { sock1.enviar_flag(FAIL); }
			break;
		case EDITADO:
			//TODO: Implementar
			cout << "Esto aun no esta implementado" << endl;
			break;
		case RENOMBRADO:
			exito = base_de_datos.renombrar(mod.nombre_archivo_alt, mod.nombre_archivo);
			if (exito) { sock1.enviar_flag(OK); }
			else { sock1.enviar_flag(FAIL); }
			break;
		case COPIADO:
			exito = base_de_datos.copiar(mod.nombre_archivo_alt, mod.nombre_archivo);
			if (exito){ sock1.enviar_flag(OK); }
			else { sock1.enviar_flag(FAIL); }
			break;
		default: // Ignoro si llega otra cosa
			break;
	}
	if (exito) notificar_todos(mod);
}

void ServerCommunicator::procesar_flag(PacketID flag)
{
	switch(flag)
	{
		case(LOGOUT):
				// cerrar coneccion y salir
				break;
		case(MODIFICACION):
				{
				Modificacion mod;
				sock1.recibir_modif(mod); // Recibe mod
				cout << mod << endl;
				actuar_segun_modif_recibida(mod); // Ejecuta mod
				}
				break;
		case(PEDIDO_ARCHIVO_ENTERO):
				{
					string nombre;
					sock1.recibir_msg_c_prefijo(nombre, 1);
					ifstream arch;
					base_de_datos.abrir_para_leer(nombre, arch);
					sock1.enviar_archivo(arch);
					//arch.close();
					base_de_datos.cerrar_archivo(nombre,arch);
				}
				break;
		case(PEDIDO_ARCHIVO_PARTES):
				// Devuelve "partes" de un archivo
				break;
		case(PEDIDO_HASHES_BLOQUES):
				// Devuelve todos los hashes de cada bloque del archivo
				break;
		case(PEDIDO_INDICE):
				// Devuelve el indice
				break;
		default:
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
			if (flag == LOGOUT)
			{
				correr = false;
				cout << "Cliente cerro sesion" << endl;
			}
			else procesar_flag(flag);
		}
		catch (exception &e)
		{
			correr = false;
			cout << "Se perdio la conexion con el cliente" << endl;
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
	Lock temp (mutex);
	sock2.enviar_modif(mod);
}
