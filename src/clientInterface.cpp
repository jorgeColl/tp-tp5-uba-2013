#include "clientInterface.h"
#include <string>
#include <sstream>
#include <gtkmm.h>
#include <glibmm.h>

ClientInterface::ClientInterface()
{
	builder = Gtk::Builder::create_from_file(USER_GLADE);
	builder->get_widget("winClient", window);
	window->signal_hide().connect(sigc::mem_fun(this,&ClientInterface::cerrar));
	//window->signal_delete_event().connect(sigc::mem_fun(this,&ClientInterface::cerrar));

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
	Gtk::Entry* entry_server = NULL;
	Gtk::Entry* entry_puerto1 = NULL;
	Gtk::Entry* entry_puerto2 = NULL;
	Gtk::Entry* entry_intrevalo_polling = NULL;
	Gtk::FileChooserButton* chooser = NULL;
	builder->get_widget("eServ", entry_server);
	builder->get_widget("ePort1", entry_puerto1);
	builder->get_widget("ePort2", entry_puerto2);
	builder->get_widget("ePoll", entry_intrevalo_polling);
	builder->get_widget("eDir", chooser);
	// Leo del archivo, si no existe lo creo, si no se puede, cargo de un stringstream
	istream *stream;
	stringstream alternativa;
	ifstream arch;
	arch.open(ARCH_PREFS_CLI);
	if (!arch.is_open())
	{
		ofstream archOutput;
		archOutput.open(ARCH_PREFS_CLI);
		archOutput << IP_DEF << "\n" << PORT1_DEF <<  "\n" << PORT2_DEF
				<< "\n" << POLLING_DEF <<  "\n" << chooser->get_current_folder();
		archOutput.close();
		alternativa << IP_DEF << "\n" << PORT1_DEF <<  "\n" << PORT2_DEF
				<< "\n" << POLLING_DEF <<  "\n" << chooser->get_current_folder();
		stream = &alternativa;
	}
	else stream = &arch;
	string buffer;
	// IP Servidor
	*stream >> buffer;
	entry_server->set_text(buffer);
	// Puerto 1
	*stream >> buffer;
	entry_puerto1->set_text(buffer);
	// Puerto 2
	*stream >> buffer;
	entry_puerto2->set_text(buffer);
	// Intervalo de polling
	*stream >> buffer;
	entry_intrevalo_polling->set_text(buffer);
	// Directorio
	*stream >> buffer;
	chooser->set_current_folder(buffer);
	arch.close();
}

void ClientInterface::correr()
{
	Gtk::Main::run(*window);
}

void ClientInterface::cerrar()
{
	// No hago nada si no esta marcado el boton
	Gtk::CheckButton* check = NULL;
	builder->get_widget("chkGuardar", check);
	if (!check->get_active()) return;

	Gtk::Entry* entry_server = NULL;
	Gtk::Entry* entry_puerto1 = NULL;
	Gtk::Entry* entry_puerto2 = NULL;
	Gtk::Entry* entry_int_polling = NULL;
	Gtk::FileChooserButton* chooser;
	builder->get_widget("eServ", entry_server);
	builder->get_widget("ePort1", entry_puerto1);
	builder->get_widget("ePort2", entry_puerto2);
	builder->get_widget("ePoll", entry_int_polling);
	builder->get_widget("eDir", chooser);
	// Guarda ultima configuracion
	ofstream input;
	input.open(ARCH_PREFS_CLI);
	if (input.is_open())
	{
		input << entry_server->get_text() << "\n";
		input << entry_puerto1->get_text() << "\n";
		input << entry_puerto2->get_text() << "\n";
		input << entry_int_polling->get_text() << "\n";
		input << chooser->get_current_folder();
	} // Falla silenciosamente si no se puede abrir el archivo
	input.close();
}

void ClientInterface::login()
{
	Gtk::Entry* entry_user = NULL;
	Gtk::Entry* entry_password = NULL;
	Gtk::Entry* entry_server = NULL;
	Gtk::Entry* entry_puerto1 = NULL;
	Gtk::Entry* entry_puerto2 = NULL;
	Gtk::Entry* entry_int_polling = NULL;
	Gtk::FileChooserButton* chooser;

	builder->get_widget("eUser", entry_user);
	std::cout << "Usuario: " << entry_user->get_text() << std::endl;
	builder->get_widget("ePass", entry_password);
	// No imprimo la password por pantalla porque es "secreta"
	builder->get_widget("eServ", entry_server);
	std::cout << "Server: " << entry_server->get_text() << std::endl;
	builder->get_widget("ePort1", entry_puerto1);
	std::cout << "Puerto 1: " << entry_puerto1->get_text() << std::endl;
	builder->get_widget("ePort2", entry_puerto2);
	std::cout << "Puerto 2: " << entry_puerto2->get_text() << std::endl;
	builder->get_widget("ePoll", entry_int_polling);
	std::cout << "Polling: " << entry_int_polling->get_text() << std::endl;
	builder->get_widget("eDir", chooser);
	std::cout << "Carpeta: " << chooser->get_current_folder() << std::endl;

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
		// Si algo falla muestro un mensaje
		Gtk::MessageDialog msg(*window, e.what(), false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE );
		msg.run();
		Gtk::Label* logged;
		builder->get_widget("lConex", logged);
		logged->set_text("No conectado");
	}


}
