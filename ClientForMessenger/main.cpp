#include "clientformessenger.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ClientForMessenger w(nullptr);
    w.show();
    return a.exec();
}
