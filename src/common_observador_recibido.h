/*
 * observador_recibido.h
 *
 *  Created on: 18/06/2013
 *      Author: jorge
 */

#ifndef OBSERVADOR_RECIBIDO_H_
#define OBSERVADOR_RECIBIDO_H_
#include "common_thread.h"
#include "common_mutex.h"
#include "common_socket_prot.h"
#include "defines.h"
using namespace std;
class ObserbadorRecibido: public Thread {
	size_t contador;
public:
	ObserbadorRecibido(){
		contador=0;
		correr=false;
	}
	void ejecutar() {
		/* QUEDA CONGELADO HASTA NUEVO AVISO
		correr=true;

		while (correr) {
			for (size_t i = 0; i < INTERVALO_GRAFICO; ++i) {
				if (correr == false) {
					return;
				}
				sleep(1);
			}
			ofstream* out;
			if(contador<=500){
				out = new ofstream(ARCHIVO_GRAFICO1, ofstream::app);
			}else{
				out = new ofstream(ARCHIVO_GRAFICO2, ofstream::app);
				if(contador>1000){
					contador=0;
					ofstream temp(ARCHIVO_GRAFICO1);
					temp.close();
				}
			}


			*out << time(NULL);
			*out <<" ";
			*out << SocketProt::get_and_reset_cantidad_recibida();
			*out << endl;
			out->close();
			delete(out);

		}
		*/
	}


};

#endif /* OBSERVADOR_RECIBIDO_H_ */
