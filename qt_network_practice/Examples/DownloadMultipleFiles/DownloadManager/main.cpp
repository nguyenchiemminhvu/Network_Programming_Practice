#include <QtCore>
#include <QObject>
#include <QtNetwork>
#include <QSslError>

#include <QString>
#include <QStringList>
#include <QDebug>

#include "downloadmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QStringList args = app.arguments();
    args.pop_front();
    if (args.isEmpty())
    {
        qDebug() << QObject::tr("Usage: downloadmanager url1 [url2 ... urlN]");
        return 0;
    }

    DownloadManager dm;
    dm.Append(args);

    QObject::connect(&dm, &DownloadManager::Finished, &app, &QCoreApplication::quit);

    return app.exec();
}

