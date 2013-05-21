#include "clientInterface.h"
#include <string>

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
	Gtk::Entry* entry = 0;
	builder->get_widget("eUser", entry);
	std::cout << "Usuario: " << entry->get_text() << std::endl;
	builder->get_widget("ePass", entry);
	std::cout << "Passoword: " << entry->get_text() << std::endl;
	builder->get_widget("eServ", entry);
	std::cout << "Server: " << entry->get_text() << std::endl;
	builder->get_widget("ePort", entry);
	std::cout << "Puerto: " << entry->get_text() << std::endl;
	builder->get_widget("ePoll", entry);
	std::cout << "Polling: " << entry->get_text() << std::endl;
	Gtk::FileChooserButton* chooser;
	builder->get_widget("eDir", chooser);
	std::cout << "Carpeta: " << chooser->get_current_folder() << std::endl;

}
