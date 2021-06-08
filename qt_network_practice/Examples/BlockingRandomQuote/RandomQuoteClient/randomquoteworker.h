#ifndef RANDOMQUOTEWORKER_H
#define RANDOMQUOTEWORKER_H

#include <QObject>
#include <QDialog>
#include <QString>
#include <QThread>

class Random_Quote_Dialog;

class RandomQuoteWorker : public QObject
{
    Q_OBJECT

public:
    explicit RandomQuoteWorker(const QString &server, const QString &port, QObject *parent = nullptr);
    virtual ~RandomQuoteWorker();

    void BindingToThread(QThread *thread);
    void BindingToSender(Random_Quote_Dialog *sender);

signals:
    void Finished();
    void QuoteAvailable(const QString &quote);

public slots:
    void Process();

private:
    QString m_server;
    QString m_port;
};

#endif // RANDOMQUOTEWORKER_H
