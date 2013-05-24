/*
 * common_modificacion.cpp
 *
 *  Created on: 22/05/2013
 *      Author: jorge
 */

#include "common_modificacion.h"
#include "cliente_controlador.h"
using namespace std;

Modificacion::Modificacion(tipo_accion accion,string nombre_archivo)
	: accion(accion),nombre_archivo(nombre_archivo) {}

bool Modificacion::efectuar_cambios(ClienteControlador& cliente) {
/*
	switch (accion) {
	case SUBIR_NUEVO_ARCHIVO: {
		return cliente.enviar_nuevo_archivo(nombre_archivo);
		break;
	}
	case BAJAR_NUEVO_ARCHIVO: {
		return cliente.pedir_nuevo_archivo(nombre_archivo);
		break;
	}
	case BORRAR_ARCHIVO_LOCAL: {
		return cliente.borrar_archvio(nombre_archivo);
		break;
	}
	case MANDAR_A_BORRAR_ARCHIVO: {
		return cliente.mandar_a_borrar(nombre_archivo);
		break;
	}
	case SUBIR_MOD_ARCHIVO: {
		return cliente.mandar_modificacion(nombre_archivo);
		break;
	}
	case BAJAR_MOD_ARCHIVO: {
		return cliente.pedir_modificacion(nombre_archivo);
		break;
	}
	}*/
	return true;
}
