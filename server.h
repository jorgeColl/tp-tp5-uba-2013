/*
 * server.h
 *
 *  Created on: 19/05/2013
 *      Author: jorge
 */

#ifndef SERVER_H_
#define SERVER_H_
/** @class Server server.h
 * @brief clase que se encarga de las acciones que puede hacer un server
 */
class Server {

public:
	/** @brief inicia un thread que se encarga de acceptar las conexiones entrantes */
	void start();
	/** @brief indica al thread que acepta las conexiones que termine de ejecutarse */
	void stop();
};



#endif /* SERVER_H_ */
