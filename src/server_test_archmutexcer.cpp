//#define test_archmutexcer
#ifndef test_archmutexcer
using namespace std;
#include "common_arch_mutexcer.h"
/*
 * Casos de prueba:
 * 1) creacion de archmutexcer y printear el estado interno viendo si creo un mutex para cada archivo
 * 2) creacion de archmutexcer y peticion de dos instancias de la misma clase , luego eliminacion de las dos instancias
 * 3) creacion de dos instancias de la misma clase, peticion de un mismo archivo a la vez
 * Nota: todos los casos se probaron con valgrind(se estan probando)
 */
int main() {
	string dir("/home/jorge/workspace2/tp5 tallerg/lugar de pruebas");

	cout<<"PRUEBA 1"<<endl;

	cout<<"generando instancia"<<endl;
	ArchMutexcer* archm = ArchMutexcer::generar_archmutexcer(dir.c_str());

	cout<<"instancia generada"<<endl;
	cout<<"mostando contenido: "<<endl;
	cout <<(*archm);
	cout<<"fin contenido"<<endl;

	Mutex* m = archm->get_mutex("doc1.txt");
	if(m==NULL){
		cout<<"MAL!!: m es NULL"<<endl;
	}

	{
	Lock t1(*m);
	//Lock t2(*m);
	}
	delete archm;

	cout<<endl<<"PRUEBA 2"<<endl;
	cout << "generando 2 instancias del mismo dir" << endl;
	SmartP smpt1 (ArchMutexcer::generar_archmutexcer(dir.c_str()));
	SmartP smpt2 (ArchMutexcer::generar_archmutexcer(dir.c_str()));
	SmartP smpt3 (ArchMutexcer::generar_archmutexcer(dir.c_str()));

	/*if(smpt1.data()==smpt2.data()){
		cout<<"BIEN!!: los punteros son iguales"<<endl;
	}else{
		cout<<"MAL!! : los punteros son distintos"<<endl;
	}*/
	cout<<"estado intero"<<endl;
	cout<<smpt1.data();
	string doc1 = dir+"doc1.txt";
	Mutex* mutx = smpt1.data().get_mutex( "doc1.txt" );
	if(mutx == NULL){
		cout <<"FALLA: devolvio mutx NULL"<<endl;
	}


	{
	cout<<"aca se deberia trabar"<<endl;
	//Lock l1 (*mutx);
	Lock l2 (*mutx);
	cout<<"si estan lo dos esto no deveria aparecer"<<endl;
	}

	cout<<"fin"<<endl;
}





#endif
