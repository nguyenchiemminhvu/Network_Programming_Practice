#include "dtls_dialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DTLS_Dialog w;
    w.show();

    return a.exec();
}
