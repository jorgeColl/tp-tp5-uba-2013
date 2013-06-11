
//#define prueba_de_base_de_datos_usuario
#ifndef prueba_de_base_de_datos_usuario
#include "common_base_de_datos_usuario.h"
#include "common_modificacion.h"
#include <iostream>
using namespace std;
int main(int args, char** argv){
	BaseDeDatosUsuario Bdd("/home/jorge/workspace2/tp5 tallerg/src/base_de_datos_usuario");
	//Bdd.abrir();
	// !los nombres sin espacios!!
	Bdd.agregar_usuario("pepito","xlanoche");
	Bdd.agregar_usuario("juanadearco", "tengounarcho");
	cout<<"tiene que decir 1(true) : "<<Bdd.usuario_contrasenia_correcto("pepito","xlanoche")<<endl;

	Bdd.guardar_a_disco();

	BaseDeDatosUsuario Bdd2("/home/jorge/workspace2/tp5 tallerg/src/base_de_datos_usuario");
	Bdd2.abrir();
	cout<<"tiene que decir 1(true) : "<<Bdd2.usuario_contrasenia_correcto("pepito","xlanoche")<<endl;
	cout<<"tiene que decir 0(false) : "<<Bdd2.usuario_contrasenia_correcto("shakira","tumama")<<endl;
	Bdd2.agregar_usuario("laaaaa","la");
	Bdd2.guardar_a_disco();

	BaseDeDatosUsuario Bdd3("/home/jorge/workspace2/tp5 tallerg/src/base_de_datos_usuario");
	Bdd3.abrir();
	Bdd3.eliminar_usuario("pepito");
	// simplemente dejo que se elimine solo para no guardar los cambios a disco
	//Bdd3.guardar_a_disco();


}



#endif
