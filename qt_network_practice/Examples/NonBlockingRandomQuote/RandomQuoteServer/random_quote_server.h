#ifndef RANDOM_QUOTE_SERVER_H
#define RANDOM_QUOTE_SERVER_H

#include <QTcpServer>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QThread>

class Random_Quote_Server : public QTcpServer
{
    Q_OBJECT

public:
    explicit Random_Quote_Server(QObject *parent = nullptr);
    virtual ~Random_Quote_Server();

protected:
    virtual void incomingConnection(qintptr socket_descriptor) override;

signals:

public slots:

private:
    void InitRandom();
    void InitQuotes();
    void EstablishSignalsAndSlots();

    int GetRandom(int low, int high) const;

private:
    QStringList m_quotes;
};

#endif // RANDOM_QUOTE_SERVER_H
