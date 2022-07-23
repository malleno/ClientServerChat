#include "serverformessenger.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ServerForMessenger w(25);
    w.show();
    return a.exec();
}
