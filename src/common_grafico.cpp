
#include "common_grafico.h"

Grafico::Grafico(): ancho(0.42), largo(0.05) {

  max_medida=0;

  test_aux = 2;
  bajando = false;

  // cosas raras para gtkmm
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &Grafico::on_timeout), 1000 );
  #ifndef GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
  //Connect the signal handler if it isn't already a virtual method override:
  signal_expose_event().connect(sigc::mem_fun(*this, &Grafico::on_expose_event), false);
  #endif //GLIBMM_DEFAULT_SIGNAL_HANDLERS_ENABLED
}

void Grafico::dibujar_division_x(Cairo::RefPtr < Cairo::Context >& cr, size_t cant_div, double offsetx) {
	double distancia = 1/double(cant_div);
	for(size_t i=0;i<cant_div;++i){
		cr->set_line_width(0.009);
		cr->move_to(i*distancia+offsetx,1);
		cr->line_to(i*distancia+offsetx,1-largo);
		cr->stroke();
	}
}
void Grafico::dibujar_division_y(Cairo::RefPtr < Cairo::Context >& cr, size_t cant_div, double offsety) {
	double distancia = 1/double(cant_div);
	for(size_t i=0;i<cant_div;++i){
		cr->set_line_width(0.009);
		cr->move_to(0,i*distancia+offsety);
		cr->line_to(largo,i*distancia+offsety);
		cr->stroke();
	}
}


bool Grafico::on_expose_event(GdkEventExpose* event) {
	// This is where we draw on the window
	Glib::RefPtr<Gdk::Window> window = get_window();
	if (window) {
		Gtk::Allocation allocation = get_allocation();
		const int width = allocation.get_width();
		const int height = allocation.get_height();

		Cairo::RefPtr < Cairo::Context > cr = window->create_cairo_context();

		if (event) {
			// clip to the area indicated by the expose event so that we only
			// redraw the portion of the window that needs to be redrawn
			cr->rectangle(event->area.x, event->area.y, event->area.width, event->area.height);
			cr->clip();
		}

    cr->scale(width, height);

    //long long unsigned int tam_medido = tamCarpeta(DIR_DEF_SERV);
    if(test_aux < 15 && bajando == false){
    	test_aux++;
    }else{
    	bajando = true;
    	test_aux--;
    	if(test_aux==1){
    		bajando=false;
    	}
    }
    float tam_medido = test_aux;
    if (medidas.size() < CANT_MEDIDAS) {
    	for(size_t i=0;i<CANT_MEDIDAS;++i){
    		medidas.push_back(tam_medido);
    	}
	}else{
		medidas.pop_front();
		medidas.push_back(tam_medido);
	}
    if(tam_medido>max_medida){
    	max_medida = tam_medido;
    	max_medida += max_medida/100;
    }else{
    	list<double>::iterator it;
    	max_medida=0;
    	for(it = medidas.begin();it!=medidas.end();++it){
    		if(max_medida<*it){
    			max_medida = *it;
    		}
    	}
    	max_medida += max_medida/100;
    }
    cout<<"tam medido:"<<tam_medido<<endl;

    float espacio = 1/float(CANT_MEDIDAS-1);
    float x=0;
    // genero ejes de coordenadas
    float offset_x = 0.02;
    float offset_y = 0.02;
    cr->set_line_width(0.0059);
    cr->move_to(0,1-offset_y);
    cr->line_to(1,1-offset_y);
    cr->move_to(offset_x,1);
    cr->line_to(offset_x,0);
    cr->stroke();

	this->dibujar_division_x(cr, CANT_MEDIDAS, offset_x);
	this->dibujar_division_y(cr, CANT_MEDIDAS, offset_y);


    // genero linea de graficos
    cr->set_line_width(0.009);
    list<double>::iterator it;
    it = medidas.begin();
    cout<<"move to:"<<0+offset_x<<" , "<<1-(float(*it)/float(max_medida))<<endl;
    cr->move_to(0+offset_x,1-(*it/max_medida)+offset_y);
    it++;
    for(;it!=medidas.end();++it){
    	x+=espacio;
    	cout<<"line to: "<<x<<" , "<<1-(*it/max_medida)<<endl;
		cr->line_to(x,1-(*it/max_medida));

    }
    cr->stroke();


 }

  return true;
}

bool Grafico::on_timeout()
{
    // force our program to redraw the entire grafico
    Glib::RefPtr<Gdk::Window> win = get_window();
    if (win)
    {
        Gdk::Rectangle r(0, 0, get_allocation().get_width(),
                get_allocation().get_height());
        win->invalidate_rect(r, false);
    }
    return true;
}

