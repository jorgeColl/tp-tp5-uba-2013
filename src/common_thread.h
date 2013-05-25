#ifndef COMMONTHREAD_H_
#define COMMONTHREAD_H_

#include <pthread.h>
#include <sys/types.h>
#include <iostream>

/**
 * @class Thread commonThread.h "commonThread.h"
 * @brief clase abstracta para implementar objetos que corren en threads
 */
class Thread{
	public:
	  Thread();
	  /**
	   * @brief Pone a andar el thread
	   */
	  virtual void start();
	  /**
	   * @brief Le pide al thread gentilmente que se detenga
	   */
	  virtual void stop();
	  /**
	   * @brief joinea el thread
	   */
	  virtual void join();
	  /**
	   * @brief Duerme el thread por un cierto tiempo
	   * @param segundos Segundos que dormira el thread aproximadamente
	   * @return true si pasaron los segundos, false si fue interrumpido
	   */
	  virtual bool sleep(unsigned int segundos);
	  virtual ~Thread(){};
	  /**
	   * @var Indica si al thread se le ha pedido que se detenga
	   */
	  bool corriendo ;
	private:
	  /**
	   * @brief Metodo virtual a modificar para cada objeto que herede
	   */
	  virtual void run() = 0;
	  pthread_t cThread;
	  static void* callback(void* thread)
	  {
		((Thread*)thread)->run();
		return 0;
	  }
	  Thread(const Thread &otroThread);
};

#endif /* COMMONTHREAD_H_ */
