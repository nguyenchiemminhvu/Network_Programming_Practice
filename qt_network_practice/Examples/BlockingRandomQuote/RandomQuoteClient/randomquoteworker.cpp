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


    emit QuoteAvailable("");
    emit Finished();
}
