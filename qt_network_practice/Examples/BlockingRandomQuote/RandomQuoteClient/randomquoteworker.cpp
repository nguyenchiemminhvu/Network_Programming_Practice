#include "randomquoteworker.h"

RandomQuoteWorker::RandomQuoteWorker(QObject *parent) : QObject(parent)
{

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

void RandomQuoteWorker::Process()
{


    emit Finished();
}
