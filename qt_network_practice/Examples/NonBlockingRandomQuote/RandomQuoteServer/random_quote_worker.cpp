#include "random_quote_worker.h"
#include "random_quote_server.h"

Random_Quote_Worker::Random_Quote_Worker(int soc, const QString &quote, QObject *parent)
    : QObject(parent)
{
    socket_descriptor = soc;
    quote_to_send = quote;
}

Random_Quote_Worker::~Random_Quote_Worker()
{

}

void Random_Quote_Worker::BindingToItself()
{
    connect(this, &Random_Quote_Worker::Finished, this, &Random_Quote_Worker::deleteLater);
    connect(this, &Random_Quote_Worker::Error, this, &Random_Quote_Worker::SomethingWrong);
}

void Random_Quote_Worker::BindingToThread(QThread *thread)
{
    connect(thread, &QThread::started, this, &Random_Quote_Worker::Process);
    connect(this, &Random_Quote_Worker::Finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
}

void Random_Quote_Worker::SomethingWrong(int error_code, const QString &message)
{
    qDebug() << tr("#%1 - %2").arg(error_code).arg(message);
    emit Finished();
}

void Random_Quote_Worker::Process()
{


    emit Finished();
}
