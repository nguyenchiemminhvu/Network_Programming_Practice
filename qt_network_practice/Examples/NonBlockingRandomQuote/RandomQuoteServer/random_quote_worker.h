#ifndef RANDOM_QUOTE_WORKER_H
#define RANDOM_QUOTE_WORKER_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QDebug>

class Random_Quote_Server;

class Random_Quote_Worker : public QObject
{
    Q_OBJECT

public:
    explicit Random_Quote_Worker(int soc, const QString &quote, QObject *parent = nullptr);
    virtual ~Random_Quote_Worker();

    void BindingToItself();
    void BindingToThread(QThread *thread);

signals:
    void Error(int error_code, const QString &message);
    void Finished();

public slots:
    void SomethingWrong(int error_code, const QString &message);
    void Process();

private:
    int socket_descriptor;
    QString quote_to_send;
};

#endif // RANDOM_QUOTE_WORKER_H
