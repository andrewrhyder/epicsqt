/* rad.cpp
 *
 */

#include <QtCore/QCoreApplication>
#include <rad_control.h>

int main (int argc, char *argv[]) {

   QCoreApplication app(argc, argv);

   new Rad_Control ();
   return app.exec ();
}

// end
