#ifndef COMMON_GRAFICO_H_
#define COMMON_GRAFICO_H_
#include <gtkmm/drawingarea.h>
#include <ctime>
#include <cmath>
#include <cairomm/context.h>
#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include "common_util.h"
#include "defines.h"
#include <list>
#include <iostream>
using namespace std;

class Grafico : public Gtk::DrawingArea {
public:
  Grafico();
  virtual ~Grafico(){};

protected:

  void dibujar_division_x(Cairo::RefPtr < Cairo::Context >& cr, size_t cant_div , double offsetx);
  void dibujar_division_y(Cairo::RefPtr < Cairo::Context >& cr, size_t cant_div , double offsety);
  //Override default signal handler:
  virtual bool on_expose_event(GdkEventExpose* event);

  bool on_timeout();
  list<double> medidas;
  double ancho;
  double largo;
  double max_medida;
  //variables de testing
  double test_aux;
  bool bajando;

};



#endif /* GRAFICO_H_ */
