#include "qaccviewsensorserveur.h"
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QAccViewSensorServeur w;
    w.show();
    return a.exec();
}
