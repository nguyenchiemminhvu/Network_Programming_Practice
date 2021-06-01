#include "broadcastdialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    BroadcastDialog w;
    w.show();

    return a.exec();
}
