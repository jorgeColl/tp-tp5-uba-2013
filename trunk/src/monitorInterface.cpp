#include "monitorInterface.h"
#include "common_util.h"
#include "defines.h"

MonitorInterface::MonitorInterface()
{
	builder = Gtk::Builder::create_from_file(MONIT_GLADE);
	builder->get_widget("winMonitor", window);

	builder->get_widget("eP1", entry_puerto1);
	builder->get_widget("eP2", entry_puerto2);
	builder->get_widget("dir", chooser_dir);
	builder->get_widget("eCreaUsNom", entry_nombre_reg);
	builder->get_widget("eCreaUsPass", entry_pass_reg);
	builder->get_widget("eElimUsNom", entry_nombre_borr);
	builder->get_widget("dbDir", db_dir);

	builder->get_widget("bNuevoUsr", pBotNeuvUs);
	builder->get_widget("bElimUsr", pBotElimUS);
	builder->get_widget("bGuardarCfg", pBotGuardCfg);
	builder->get_widget("bCarga", pCargar);
	builder->get_widget("bGuardar", pGuardar);

	pBotNeuvUs->signal_clicked().connect(sigc::mem_fun(this,&MonitorInterface::registrarUsuario));
	pBotElimUS->signal_clicked().connect(sigc::mem_fun(this,&MonitorInterface::borrarUsuario));
	pBotGuardCfg->signal_clicked().connect(sigc::mem_fun(this,&MonitorInterface::guardarPreferencias));
	pCargar->signal_clicked().connect(sigc::mem_fun(this,&MonitorInterface::cargarDB));
	pGuardar->signal_clicked().connect(sigc::mem_fun(this,&MonitorInterface::guardarDB));

	bd_usr = new BaseDeDatosUsuario(chooser_dir->get_current_folder());
	cargarPreferencias();
}

void MonitorInterface::correr()
{
	Gtk::Main::run(*window);
}

void MonitorInterface::cargarDB()
{
	try
	{
		bd_usr->setPath(unirPath(db_dir->get_current_folder(),DB_USU_ARCH));
		bd_usr->abrir();
		Gtk::MessageDialog msg(*window, "Se ha abierto la base de datos.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
		msg.run();
	}
	catch (exception &e)
	{
		Gtk::MessageDialog msg(*window, e.what(), false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE );
		msg.run();
	}
}

void MonitorInterface::guardarDB()
{
	try
	{
		bd_usr->guardar_a_disco();
		Gtk::MessageDialog msg(*window, "Se han guardado los cambios.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
		msg.run();
	}
	catch (exception &e)
	{
		Gtk::MessageDialog msg(*window, e.what(), false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE );
		msg.run();
	}
}

void MonitorInterface::cargarPreferencias()
{
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
		db_dir->set_current_folder(dir);
	}
	else
	{
		entry_puerto1->set_text(PUERTO1_DEF);
		entry_puerto2->set_text(PUERTO2_DEF);
	}
	arch.close();
}

void MonitorInterface::guardarPreferencias()
{
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
}

void MonitorInterface::registrarUsuario()
{
	try
	{
		bd_usr->agregar_usuario(entry_nombre_reg->get_text().c_str(), entry_pass_reg->get_text().c_str());
		entry_nombre_reg->set_text("");
		entry_pass_reg->set_text("");
		Gtk::MessageDialog msg(*window, "Registro exitoso.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
		msg.run();
	}
	catch (exception &e)
	{
		Gtk::MessageDialog msg(*window, e.what(), false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE );
		msg.run();
	}
}

void MonitorInterface::borrarUsuario()
{
	try
	{
		bd_usr->eliminar_usuario(entry_nombre_borr->get_text().c_str());

		entry_nombre_borr->set_text("");
		Gtk::MessageDialog msg(*window, "Borrado exitoso.", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK);
		msg.run();
	}
	catch (exception &e)
	{
		Gtk::MessageDialog msg(*window, e.what(), false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE );
		msg.run();
	}
}

MonitorInterface::~MonitorInterface()
{
	delete bd_usr;
}
