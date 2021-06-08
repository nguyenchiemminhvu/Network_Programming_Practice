#include "randomquoteworker.h"
#include "random_quote_dialog.h"

RandomQuoteWorker::RandomQuoteWorker(const QString &server, const QString &port, QObject *parent) : QObject(parent)
{
    m_server = server;
    m_port = port;
}

RandomQuoteWorker::~RandomQuoteWorker()
{

}

void RandomQuoteWorker::BindingToItself()
{
    connect(this, &RandomQuoteWorker::Error, this, &RandomQuoteWorker::OnErrorHappenned);
}

void RandomQuoteWorker::BindingToThread(QThread *thread)
{
    connect(thread, &QThread::started, this, &RandomQuoteWorker::Process);
    connect(this, &RandomQuoteWorker::Finished, thread, &QThread::quit);
    connect(this, &RandomQuoteWorker::Finished, this, &RandomQuoteWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    this->moveToThread(thread);
}

void RandomQuoteWorker::BindingToSender(Random_Quote_Dialog *sender)
{
    connect(this, &RandomQuoteWorker::QuoteAvailable, sender, &Random_Quote_Dialog::DisplayQuote);
    connect(this, &RandomQuoteWorker::Finished, sender, &Random_Quote_Dialog::ReadyToSendRequest);
}

void RandomQuoteWorker::Process()
{
    quint16 port = m_port.toInt();
    QTcpSocket *client = new QTcpSocket();
    client->connectToHost(m_server, port);
    if (!client->waitForConnected())
    {
        emit Error(1, client->errorString());
        emit Finished();
        return;
    }

    if (!client->waitForReadyRead())
    {
        emit Error(2, client->errorString());
        emit Finished();
        return;
    }

    QString quote_to_display;
    QDataStream in_stream(client);
    in_stream.startTransaction();
    in_stream >> quote_to_display;

    emit QuoteAvailable(quote_to_display);
    emit Finished();
}

void RandomQuoteWorker::OnErrorHappenned(int soc_error, const QString &message)
{
    qDebug() << tr("Error: %1 Message: %2").arg(soc_error).arg(message);
}
