#ifndef RANDOMQUOTEWORKER_H
#define RANDOMQUOTEWORKER_H

#include <QObject>
#include <QThread>

class RandomQuoteWorker : public QObject
{
    Q_OBJECT

public:
    explicit RandomQuoteWorker(QObject *parent = nullptr);
    virtual ~RandomQuoteWorker();

    void BindingToThread(QThread *thread);

signals:
    void Finished();

public slots:
    void Process();

private:

};

#endif // RANDOMQUOTEWORKER_H
