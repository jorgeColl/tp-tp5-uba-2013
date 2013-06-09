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
	cargarPreferencias();
}

void ClientInterface::cargarPreferencias()
{
	//TODO: Leer de archivo
	Gtk::Entry* entry_server = NULL;
	Gtk::Entry* entry_puerto1 = NULL;
	Gtk::Entry* entry_puerto2 = NULL;
	Gtk::Entry* entry_intrevalo_polling = NULL;
	Gtk::FileChooserButton* chooser = NULL;

	std::ifstream arch;
	arch.open(".preferences", arch.in);
	if (!arch.is_open()) {
		arch.close();
		ofstream input;
		input.open(".preferences");
		input << "127.0.0.1 12700 12701 5";
		input.close();
		arch.open(".preferences");
	}

	char buffer[50];

	builder->get_widget("eServ", entry_server);
	//"127.0.0.1"
	arch >> buffer;
	entry_server->set_text(buffer);
	builder->get_widget("ePort1", entry_puerto1);
	//"12700"
	arch >> buffer;
	entry_puerto1->set_text(buffer);
	builder->get_widget("ePort2", entry_puerto2);
	//"12701"
	arch >> buffer;
	entry_puerto2->set_text(buffer);
	builder->get_widget("ePoll", entry_intrevalo_polling);
	//"5"
	arch >> buffer;
	entry_intrevalo_polling->set_text(buffer);
	//Sin archivo esta parte no tiene chiste
	//builder->get_widget("eDir", chooser);
	//chooser->set_current_folder(chooser->get_current_folder().append("/user1"));
}

void ClientInterface::correr()
{
	Gtk::Main::run(*window);
}

//TODO: Hook verdadero
#include <iostream>
void ClientInterface::login()
{
	Gtk::Entry* entry_user = NULL;
	Gtk::Entry* entry_password = NULL;
	Gtk::Entry* entry_server = NULL;
	Gtk::Entry* entry_puerto1 = NULL;
	Gtk::Entry* entry_puerto2 = NULL;
	Gtk::Entry* entry_int_polling = NULL;

	builder->get_widget("eUser", entry_user);
	std::cout << "Usuario: " << entry_user->get_text() << std::endl;
	builder->get_widget("ePass", entry_password);
	std::cout << "Password: " << entry_password->get_text() << std::endl;
	builder->get_widget("eServ", entry_server);
	std::cout << "Server: " << entry_server->get_text() << std::endl;
	builder->get_widget("ePort1", entry_puerto1);
	std::cout << "Puerto 1: " << entry_puerto1->get_text() << std::endl;
	builder->get_widget("ePort2", entry_puerto2);
	std::cout << "Puerto 2: " << entry_puerto2->get_text() << std::endl;
	builder->get_widget("ePoll", entry_int_polling);
	std::cout << "Polling: " << entry_int_polling->get_text() << std::endl;
	Gtk::FileChooserButton* chooser;
	builder->get_widget("eDir", chooser);
	std::cout << "Carpeta: " << chooser->get_current_folder() << std::endl;

	// guarda ultima configuracion
	ofstream input;
	input.open(".preferences",input.out);
	if (!input.is_open()) { throw ios_base::failure("el archivo no se abrio"); }
	input << entry_server->get_text();
	input << " ";
	input << entry_puerto1->get_text();
	input << " ";
	input << entry_puerto2->get_text();
	input << " ";
	input << entry_int_polling->get_text();
	input.close();
	// Intenta loguearse
	try
	{
		cli.set_directorio(chooser->get_current_folder());
		cli.login(entry_server->get_text(), entry_puerto1->get_text(),entry_puerto2->get_text(),
				entry_user->get_text(), entry_password->get_text(), entry_int_polling->get_text());
		//Login exitoso
		Gtk::Label* logged;
		builder->get_widget("lConex", logged);
		logged->set_text("Conectado");
		cli.start();
	}
	catch (exception &e)
	{
		//TODO: Popup
		cout << "Error: " << e.what() << endl;
	}


}
