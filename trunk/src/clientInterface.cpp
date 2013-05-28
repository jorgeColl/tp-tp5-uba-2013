#include "clientInterface.h"
#include <string>
#include <gtkmm.h>
#include <glibmm.h>

ClientInterface::ClientInterface(int argc, char** argv)
{
	builder = Gtk::Builder::create_from_file(USER_GLADE);
	builder->get_widget("winClient", window);

	Gtk::Button* pButton = 0;
	builder->get_widget("bConex", pButton);
	if(pButton)
	{
	  pButton->signal_clicked().connect(sigc::mem_fun(this,&ClientInterface::login));
	}
	//TODO: Cargar parametros guardados
}

void ClientInterface::correr()
{
	Gtk::Main::run(*window);
}

//TODO: Hook verdadero
#include <iostream>
void ClientInterface::login()
{
	std::cout << "CLICK" << std::endl;

	Gtk::Entry* entry_user = 0;
	Gtk::Entry* entry_password = 0;
	Gtk::Entry* entry_server = 0;
	Gtk::Entry* entry_puerto1 = 0;
	Gtk::Entry* entry_puerto2 = 0;
	Gtk::Entry* entry_intrevalo_polling = 0;

	builder->get_widget("eUser", entry_user);
	std::cout << "Usuario: " << entry_user->get_text() << std::endl;
	builder->get_widget("ePass", entry_password);
	std::cout << "Passoword: " << entry_password->get_text() << std::endl;
	builder->get_widget("eServ", entry_server);
	std::cout << "Server: " << entry_server->get_text() << std::endl;
	builder->get_widget("ePort1", entry_puerto1);
	std::cout << "Puerto 1: " << entry_puerto1->get_text() << std::endl;
	builder->get_widget("ePort2", entry_puerto2);
	std::cout << "Puerto 2: " << entry_puerto2->get_text() << std::endl;
	builder->get_widget("ePoll", entry_intrevalo_polling);
	std::cout << "Polling: " << entry_intrevalo_polling->get_text() << std::endl;
	Gtk::FileChooserButton* chooser;
	builder->get_widget("eDir", chooser);
	std::cout << "Carpeta: " << chooser->get_current_folder() << std::endl;

	// Intenta loguearse
	try
	{
		cli.set_directorio(chooser->get_current_folder());
		cli.login(entry_server->get_text(), entry_puerto1->get_text(),
				entry_puerto2->get_text(), entry_user->get_text(), entry_password->get_text());
		//Login exitoso
		Gtk::Label* logged;
		builder->get_widget("lConex", logged);
		logged->set_text("Conectado");
	}
	catch (exception &e)
	{
		//TODO: Popup
		cout << e.what() << endl;
	}


}
