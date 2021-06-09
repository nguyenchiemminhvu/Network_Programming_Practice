#ifndef RANDOM_QUOTE_SERVER_H
#define RANDOM_QUOTE_SERVER_H

#include <QTcpServer>

class Random_Quote_Server : public QTcpServer
{
    Q_OBJECT

public:
    explicit Random_Quote_Server(QObject *parent = nullptr);
    virtual ~Random_Quote_Server();

    void EstablishSignalsAndSlots();

signals:

public slots:

private:

};

#endif // RANDOM_QUOTE_SERVER_H
