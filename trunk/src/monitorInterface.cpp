#include "monitorInterface.h"
#include "common_util.h"
#include "defines.h"
#include <iostream>     // std::cout, std::fixed
#include <iomanip>
#include <syslog.h>
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


	largo = double(LARGO_DIVISIONES_GRAFICO);
	max_medida = 0;
	offset_x = 0.2;
	offset_y = 0.1;

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
	/* genero ejes de coordenadas
					^
		<<offsetx>>	|
					|-------->
						^
						o
						f
						y
						^
		*/
	Glib::RefPtr<Gdk::Window> ventana= draw_area->get_window();
	if (!ventana) {
		return false;
	}
	Gtk::Allocation allocation = draw_area->get_allocation();
	const int width = allocation.get_width();
	const int height = allocation.get_height();
	Cairo::RefPtr < Cairo::Context > cr = ventana->create_cairo_context();

	// dibujo TEXTO	en el grefico EJE Y
	double distacia_y = height/double(CANT_DIVISIONES);
	syslog(LOG_DEBUG, "Distacia_y: %f", distacia_y);

	double valor =0;
	for(size_t i=1;i<CANT_DIVISIONES-2;++i){
		stringstream aux;
		string auxaux;
		// el -10 es por el tamaño de la letra, ya que se empieza a dibujar desde la pos arriba a la izquierda
		cr->move_to(width*0.02,height -i*distacia_y -offset_y*height -8);

		valor+=(max_medida/(CANT_DIVISIONES));
		aux<<std::setprecision(2)<<fixed<<(valor);
		aux>>auxaux;
		Glib::RefPtr<Pango::Layout> pl = draw_area->create_pango_layout(auxaux);
		//Glib::RefPtr<Pango::FontDescription >desc = pango_font_description_from_string("Sans Bold 8");
		//pango_layout_set_font_description(pl, desc);
		pl->show_in_cairo_context(cr);
	}
	// fin del dibujo del TEXTO EJE Y
	// dibujo eje x
	double distancia_x = width/double(CANT_DIVISIONES);
	int valor_x =-16;
	for(size_t i=0;i<CANT_DIVISIONES;++i){
		stringstream aux_x;
		string auxaux_x;
		cr->move_to(distancia_x*i+ width*offset_x-10, height-offset_y*height+4);
		aux_x<<std::setprecision(2)<<fixed<<(valor_x);
		aux_x>>auxaux_x;
		Glib::RefPtr<Pango::Layout> pl = draw_area->create_pango_layout(auxaux_x);
		pl->show_in_cairo_context(cr);
		valor_x++;

	}
	// fin dibujo ejex
	cr->move_to(width*0.015, height-offset_y*height);
	Glib::RefPtr<Pango::Layout> pl = draw_area->create_pango_layout("MB/seg");
	pl->show_in_cairo_context(cr);
	cr->scale(width, height);

	this->dibujar_ejes(cr);
	this->dibujar_division_x(cr, CANT_DIVISIONES, offset_x, offset_y);
	this->dibujar_division_y(cr, CANT_DIVISIONES, offset_y, offset_x);

	double espacio = 1 / double(medidas.size());
	double x = 0;

	// genero linea de graficos
	cr->set_line_width(0.009);
	cr->set_source_rgba(0.337, 0.612, 0.117, 0.9);
	list<double>::iterator it;
	it = medidas.begin();
	cr->move_to(0 + offset_x, 1 - (*it / max_medida) - offset_y);
	it++;
	for (; it != medidas.end(); ++it) {
		x += espacio;
		cr->line_to(x+offset_x, 1 - (*it / max_medida) -offset_y);
	}

	cr->stroke();

	return true;
}
void MonitorInterface::dibujar_ejes(Cairo::RefPtr < Cairo::Context >& cr){

	cr->set_line_width(0.0059);
	// eje x
	cr->move_to(0+offset_x, 1 - offset_y);
	cr->line_to(1, 1 - offset_y);
	// eje y
	cr->move_to(0+offset_x, 1 - offset_y);
	cr->line_to(offset_x, 0);
	cr->stroke();
}
void MonitorInterface::dibujar_division_x(Cairo::RefPtr < Cairo::Context >& cr, size_t cant_div, double& offsetx,double& offsety) {
	double distancia = 1/double(cant_div);
	for(size_t i=0;i<cant_div;++i) {
		cr->set_line_width(0.009);
		cr->move_to(i*distancia+offsetx,1-offsety+largo/2);
		cr->line_to(i*distancia+offsetx,1-offsety-largo/2);
		cr->stroke();
	}
}
void MonitorInterface::dibujar_division_y(Cairo::RefPtr < Cairo::Context >& cr, size_t cant_div, double& offsety, double& offsetx) {
	double distancia = 1/double(cant_div);
	for(size_t i=0;i<cant_div;++i){
		cr->set_line_width(0.009);
		cr->move_to(offsetx-largo/2	,i*distancia-offsety);
		cr->line_to(offsetx+largo/2	,i*distancia-offsety);
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
	syslog(LOG_DEBUG, "Tam de carpeta recien calculado: %f MB", tam_medido);
	if(CONSOLE_DEBUG == 1) cout<<"Tam de carpeta recien calculado: "<<tam_medido<<endl;
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
		syslog(LOG_DEBUG, "Mayor medida encontrada: %f", max_medida);
		max_medida+=max_medida/4;
		syslog(LOG_DEBUG, "Mayor medida usada en grafico para que quede lindo: %f", max_medida);
	}
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
