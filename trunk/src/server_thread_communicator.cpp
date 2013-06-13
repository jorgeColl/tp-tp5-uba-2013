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
	cout<<"enviando flag OK"<<endl;
	sock1.enviar_flag(OK);
	cout<<"flag enviado :)"<<endl;
	bool exito = false;
	cout<<"antes lock"<<endl;

	Lock(*smpt.data().get_mutex(mod.nombre_archivo.c_str()));
	Lock* lock = 0;
	if(mod.nombre_archivo_alt ==""){
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
					sock1.enviar_flag(OK);
				}
			}
			break;
		case BORRADO:
			exito = base_de_datos.eliminar_archivo(mod.nombre_archivo);
			if (exito)
			{
				base_de_datos.registrar_eliminado(mod.nombre_archivo);
				sock1.enviar_flag(OK);
			}
			else sock1.enviar_flag(FAIL);
			break;
		case EDITADO:
		{
			ofstream destino;
			base_de_datos.abrir_para_escribir_temporal(mod.nombre_archivo, destino);
			ifstream original;
			base_de_datos.abrir_para_leer(mod.nombre_archivo, original);
			off_t bloques = 0;
			sock1.recibirLen((char*)&bloques, sizeof(off_t));
			list<off_t> bloqPedir;
			for (off_t i = 0; i < bloques; ++i) // Reviso todos los hashes que me llegan
			{
				char buffer[BYTES_HASH];
				sock1.recibirLen(buffer, sizeof(off_t));
				string hashRecibido(buffer, BYTES_HASH);
				string hashLocal;
				MD5_bloque(original, password, i*TAM_BLOQ, TAM_BLOQ, hashLocal);
				if (hashLocal != hashRecibido) bloqPedir.push_back(i);
			}
			//Envio la lista
			size_t tam = bloqPedir.size();
			sock1.enviarLen((char*)&tam, sizeof(size_t)); // Prefijo de longitud
			for(list<off_t>::iterator it = bloqPedir.begin(); it != bloqPedir.end(); ++it)
			{
				sock1.enviarLen((char*)&(*it), sizeof(off_t));
			}
			// Hacemos el "mechado" entre los dos archivos con lo que vamos recibiendo
			char buffer[TAM_BLOQ];
			for(off_t i = 0; i < bloques; ++i)
			{
				off_t bloqNoLocal = -1;
				if (!bloqPedir.empty())
				{
					bloqPedir.front();
					bloqPedir.pop_front();
				}
				if (bloqNoLocal == i) // Copio lo que me llega por el socket
				{
					sock1.recibir_pedazo_archivo(destino, i*TAM_BLOQ, TAM_BLOQ);
				}
				else // Copio del original de ser necesario
				{
					original.read(buffer, TAM_BLOQ);
					destino.write(buffer, original.gcount());
				}
			}

			// Terminamos
			original.close();
			destino.close();
			base_de_datos.renombrar_temporal(mod.nombre_archivo);
			base_de_datos.registrar_modificado(mod.nombre_archivo);
			sock1.enviar_flag(OK);
		}
			break;
		case RENOMBRADO:
			exito = base_de_datos.renombrar(mod.nombre_archivo_alt, mod.nombre_archivo);
			if (exito)
			{
				base_de_datos.registrar_renombrado(mod.nombre_archivo, mod.nombre_archivo_alt);
				sock1.enviar_flag(OK);
			}
			else { sock1.enviar_flag(FAIL); }
			break;
		case COPIADO:
			exito = base_de_datos.copiar(mod.nombre_archivo_alt, mod.nombre_archivo);
			if (exito)
			{
				base_de_datos.registrar_copiado(mod.nombre_archivo, mod.nombre_archivo_alt);
				sock1.enviar_flag(OK);
			}
			else sock1.enviar_flag(FAIL);
			break;
		default: // Igstnoro si llega otra cosa
			break;
	}
	if (exito){
		cout<<"notificando otros clientes de la modificacion"<<endl;
		notificar_todos(mod);
	}else{
		cout<<"modificacion SIN exito , NO se notifica al resto"<<endl;
	}
	cout << "fin procesado"<<endl;
	if(mod.nombre_archivo_alt ==""){
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
					cout<<"flag es PEDIDO_ARCHIVO_ENTERO"<<endl;
					string nombre;
					sock1.recibir_msg_c_prefijo(nombre, 1);
					Lock(*smpt.data().get_mutex(nombre.c_str()));
					ifstream arch;
					base_de_datos.abrir_para_leer(nombre, arch);
					sock1.enviar_archivo(arch);
					arch.close();
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
				{
					cout<<"flag es PEDIDO_INDICE"<<endl;
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
				cout << "Cliente cerro sesion" << endl;
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
