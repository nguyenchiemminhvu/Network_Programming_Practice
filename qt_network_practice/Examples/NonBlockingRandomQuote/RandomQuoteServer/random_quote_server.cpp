#include "random_quote_server.h"


Random_Quote_Server::Random_Quote_Server(QObject *parent)
    : QTcpServer(parent)
{
    EstablishSignalsAndSlots();
}

Random_Quote_Server::~Random_Quote_Server()
{

}

void Random_Quote_Server::EstablishSignalsAndSlots()
{

}
