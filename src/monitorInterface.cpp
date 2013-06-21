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

	builder->get_widget("drawingarea1", draw_area);

	pBotNeuvUs->signal_clicked().connect(sigc::mem_fun(this,&MonitorInterface::registrarUsuario));
	pBotElimUS->signal_clicked().connect(sigc::mem_fun(this,&MonitorInterface::borrarUsuario));
	pBotGuardCfg->signal_clicked().connect(sigc::mem_fun(this,&MonitorInterface::guardarPreferencias));
	pCargar->signal_clicked().connect(sigc::mem_fun(this,&MonitorInterface::cargarDB));
	pGuardar->signal_clicked().connect(sigc::mem_fun(this,&MonitorInterface::guardarDB));
	draw_area->signal_expose_event().connect(sigc::mem_fun (*this,&MonitorInterface::graficar));
	Glib::signal_timeout().connect( sigc::mem_fun(*this, &MonitorInterface::on_timeout), INTERVALO_DE_MEDIDAS);


	largo = 0.04;
	max_medida = 0;
	// para testing
	bajando=false;

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

bool MonitorInterface::graficar(GdkEventExpose* event){

	Glib::RefPtr<Gdk::Window> ventana= draw_area->get_window();
	if (!ventana) {
		return false;
	}
	Gtk::Allocation allocation = draw_area->get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	Cairo::RefPtr < Cairo::Context > cr = ventana->create_cairo_context();

	cr->move_to(width/2,height/2);
	Glib::RefPtr<Pango::Layout> pl = draw_area->create_pango_layout("holaaa");
	pl->show_in_cairo_context(cr);



	cr->scale(width, height);

	double espacio = 1 / double(CANT_MEDIDAS);
	double x = 0;
	// genero ejes de coordenadas
	double offset_x = 0.02;
	double offset_y = 0.02;
	cr->set_line_width(0.0059);
	// eje x
	cr->move_to(0+offset_x, 1 - offset_y);
	cr->line_to(1, 1 - offset_y);
	// eje x
	cr->move_to(offset_x, 1 - offset_y);
	cr->line_to(offset_x, 0);
	cr->stroke();

	this->dibujar_division_x(cr, CANT_DIVISIONES, offset_x, offset_y);
	this->dibujar_division_y(cr, CANT_DIVISIONES, offset_y, offset_x);

	// genero linea de graficos
	cr->set_line_width(0.009);
	list<double>::iterator it;
	it = medidas.begin();
	//cout << "move to:" << 0 + offset_x << " , " << 1 - (*it / max_medida) <<endl;
	cr->move_to(0 + offset_x, 1 - (*it / max_medida) - offset_y);
	it++;
	for (; it != medidas.end(); ++it) {
		x += espacio;
		//cout <<"line to: "<<x<<" , "<<1 - (*it / max_medida)<<endl;
		cr->line_to(x+offset_x, 1 - (*it / max_medida) -offset_y);
	}

	cr->stroke();

	return true;
}
void MonitorInterface::dibujar_division_x(Cairo::RefPtr < Cairo::Context >& cr, size_t cant_div, double& offsetx,double& offsety) {
	double distancia = 1/double(cant_div);
	for(size_t i=0;i<cant_div;++i) {
		cr->set_line_width(0.009);
		cr->move_to(i*distancia+offsetx,1-offsety/2);
		cr->line_to(i*distancia+offsetx,1-largo+offsety/2);
		cr->stroke();
	}
}
void MonitorInterface::dibujar_division_y(Cairo::RefPtr < Cairo::Context >& cr, size_t cant_div, double& offsety, double& offsetx) {
	double distancia = 1/double(cant_div);
	for(size_t i=0;i<cant_div;++i){
		cr->set_line_width(0.009);
		cr->move_to(0+offsetx/2,i*distancia-offsety);
		cr->line_to(largo-offsetx/2,i*distancia-offsety);
		cr->stroke();
	}
}
bool MonitorInterface::on_timeout() {
	Glib::RefPtr<Gdk::Window> win = draw_area->get_window();
	if (win) {
		Gdk::Rectangle r(0, 0, draw_area->get_allocation().get_width(), draw_area->get_allocation().get_height());
		win->invalidate_rect(r, false);
	}

	double tam_medido = tamCarpeta(DIR_DEF_SERV);
	cout<<"tamaño carpeta: "<< tam_medido <<" MB"<<endl;
	if (medidas.size() < CANT_MEDIDAS) {
		for (size_t i = 0; i < CANT_MEDIDAS; ++i) {
			medidas.push_back(tam_medido);
		}
	} else {
		medidas.pop_front();
		medidas.push_back(tam_medido);
	}
	if (tam_medido > max_medida) {
		max_medida = tam_medido;
	} else {
		list<double>::iterator it;
		max_medida = 0;
		for (it = medidas.begin(); it != medidas.end(); ++it) {
			if (max_medida < *it) {
				max_medida = *it;
			}
		}
		max_medida+=max_medida/4;
	}
	cout << "tam medido:" << tam_medido << endl;
	return true;
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
