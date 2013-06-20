/*
 * monitor_prueba_grafico.cpp
 *
 *  Created on: 19/06/2013
 *      Author: jorge
 */
#include "common_grafico.h"
int main(int argc, char** argv)
{
   Gtk::Main kit(argc, argv);

   Gtk::Window win;
   win.set_title("Grafico");

   Grafico c;
   win.add(c);
   c.show();

   Gtk::Main::run(win);

   return 0;
}
