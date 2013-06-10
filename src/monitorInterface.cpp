#include "monitorInterface.h"

MonitorInterface::MonitorInterface()
{
	builder = Gtk::Builder::create_from_file(MONIT_GLADE);
	builder->get_widget("winMonitor", window);

	Gtk::Button* pBotNeuvUs = 0;
	builder->get_widget("bNuevoUsr", pBotNeuvUs);
	if(pBotNeuvUs) pBotNeuvUs->signal_clicked().connect(sigc::mem_fun(this,&MonitorInterface::registrarUsuario));

	Gtk::Button* pBotElimUS = 0;
	builder->get_widget("bElimUsr", pBotElimUS);
	if(pBotElimUS) pBotElimUS->signal_clicked().connect(sigc::mem_fun(this,&MonitorInterface::borrarUsuario));

	Gtk::Button* pBotGuardCfg = 0;
	builder->get_widget("bGuardarCfg", pBotGuardCfg);
	if(pBotGuardCfg) pBotGuardCfg->signal_clicked().connect(sigc::mem_fun(this,&MonitorInterface::guardarPreferencias));

	cargarPreferencias();

	Gtk::FileChooserButton* chooser_dir = 0;
	builder->get_widget("dir", chooser_dir);

	bd_usr = new BaseDeDatosUsuario(chooser_dir->get_current_folder().c_str());
	mostrarDir();
}

void MonitorInterface::mostrarDir()
{
	Gtk::FileChooserButton* chooser_dir = 0;
	builder->get_widget("dir", chooser_dir);
	Gtk::Label* dir_uso = 0;
	builder->get_widget("lBD", dir_uso);
	dir_uso->set_text(chooser_dir->get_current_folder().c_str());
}

void MonitorInterface::correr()
{
	Gtk::Main::run(*window);
}

void MonitorInterface::cargarPreferencias()
{
	Gtk::Entry* entry_puerto1 = NULL;
	Gtk::Entry* entry_puerto2 = NULL;
	Gtk::FileChooserButton* chooser_dir;
	builder->get_widget("eP1", entry_puerto1);
	builder->get_widget("eP2", entry_puerto2);
	builder->get_widget("dir", chooser_dir);
	// Guarda ultima configuracion
	ifstream arch;
	arch.open(ARCH_PREFS_SERV);
	if (arch.is_open())
	{
		string p1, p2, dir;
		arch >> p1 >> p2 >> dir;
		entry_puerto1->set_text(p1);
		entry_puerto2->set_text(p2);
		chooser_dir->set_current_folder(dir);
	}
	else
	{
		entry_puerto1->set_text(PUERTO1_DEF);
		entry_puerto2->set_text(PUERTO2_DEF);
		chooser_dir->set_current_folder(DIR_DEF);
	}
	arch.close();
}

void MonitorInterface::guardarPreferencias()
{
	Gtk::Entry* entry_puerto1 = NULL;
	Gtk::Entry* entry_puerto2 = NULL;
	Gtk::FileChooserButton* chooser_dir;
	builder->get_widget("eP1", entry_puerto1);
	builder->get_widget("eP2", entry_puerto2);
	builder->get_widget("dir", chooser_dir);
	// Guarda ultima configuracion
	ofstream output;
	output.open(ARCH_PREFS_SERV);
	if (output.is_open())
	{
		output << entry_puerto1->get_text() << "\n";
		output << entry_puerto2->get_text() << "\n";
		output << chooser_dir->get_current_folder();
		Gtk::MessageDialog msg(*window, "Operacion exitosa.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
		msg.run();
	}
	else
	{
		Gtk::MessageDialog msg(*window,"No se pudo abrir el archivo de configuraciones.",
				false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE );
		msg.run();
	}
	output.close();
	mostrarDir();
}

void MonitorInterface::registrarUsuario()
{
	Gtk::Entry* entry_nombre = NULL;
	Gtk::Entry* entry_pass = NULL;
	builder->get_widget("eCreaUsNom", entry_nombre);
	builder->get_widget("eCreaUsPass", entry_pass);
	try
	{
		bd_usr->agregar_usuario(entry_nombre->get_text().c_str(), entry_pass->get_text().c_str());
		bd_usr->guardar_a_disco();
		Gtk::MessageDialog msg(*window, "Operacion exitosa.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
		msg.run();
	}
	catch (exception &e)
	{
		Gtk::MessageDialog msg(*window, e.what(), false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE );
	}
}

void MonitorInterface::borrarUsuario()
{
	Gtk::Entry* entry_nombre = NULL;
	builder->get_widget("eCreaUsNom", entry_nombre);
	try
	{
		bd_usr->eliminar_usuario(entry_nombre->get_text().c_str());
		bd_usr->guardar_a_disco();
		Gtk::MessageDialog msg(*window, "Operacion exitosa.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
		msg.run();
	}
	catch (exception &e)
	{
		Gtk::MessageDialog msg(*window, e.what(), false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE );
	}
}

MonitorInterface::~MonitorInterface()
{
	delete bd_usr;
}
