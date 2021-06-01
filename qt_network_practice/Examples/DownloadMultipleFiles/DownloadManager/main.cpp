#include <QtCore>
#include <QtNetwork>
#include <QSslError>

#include <stdio.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    return a.exec();
}

