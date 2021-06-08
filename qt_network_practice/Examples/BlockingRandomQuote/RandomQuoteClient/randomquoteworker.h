#ifndef RANDOMQUOTEWORKER_H
#define RANDOMQUOTEWORKER_H

#include <QObject>
#include <QDialog>
#include <QString>
#include <QThread>
#include <QTcpSocket>
#include <QDataStream>

#include <QDebug>

class Random_Quote_Dialog;

class RandomQuoteWorker : public QObject
{
    Q_OBJECT

public:
    explicit RandomQuoteWorker(const QString &server, const QString &port, QObject *parent = nullptr);
    virtual ~RandomQuoteWorker();

    void BindingToItself();
    void BindingToThread(QThread *thread);
    void BindingToSender(Random_Quote_Dialog *sender);

signals:
    void Error(int, const QString&);
    void Finished();
    void QuoteAvailable(const QString &quote);

public slots:
    void Process();
    void OnErrorHappenned(int soc_error, const QString &message);

private:
    QString m_server;
    QString m_port;
};

#endif // RANDOMQUOTEWORKER_H
