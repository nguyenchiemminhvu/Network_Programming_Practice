#include "random_quote_server.h"
#include "random_quote_worker.h"


Random_Quote_Server::Random_Quote_Server(QObject *parent)
    : QTcpServer(parent)
{
    InitRandom();
    InitQuotes();
    EstablishSignalsAndSlots();
}

Random_Quote_Server::~Random_Quote_Server()
{

}

void Random_Quote_Server::incomingConnection(qintptr socket_descriptor)
{
    int quote_index = GetRandom(0, m_quotes.size() - 1);
    QString selected_quote = m_quotes[quote_index];

    QThread *thread = new QThread();
    Random_Quote_Worker *worker = new Random_Quote_Worker(socket_descriptor, selected_quote);
    worker->BindingToItself();
    worker->BindingToThread(thread);
    thread->start();
}

void Random_Quote_Server::InitRandom()
{
    QTime cur = QTime::currentTime();
    qsrand((uint)cur.msec());
}

void Random_Quote_Server::InitQuotes()
{
    m_quotes    <<  "We must be global Christians with a global vision because our God is a global God. -John Stott"
                <<  "God’s work done in God’s way will never lack God’s supplies. -J. Hudson Taylor"
                <<  "If Jesus Christ be God and died for me, then no sacrifice can be too great for me to make for Him. -C.T. Studd"
                <<  "Christians don’t tell lies they just go to church and sing them -A.W. Tozer"
                <<  "We talk of the second coming, half the world has never heard of the first. -Oswald J. Smith"
                <<  "Today Christians spend more money on dog food then missions -Leonard Ravenhill"
                <<  "He must increase, but I must decrease. -John the Baptist"
                <<  "Here am I. Send me. -Isaiah"
                <<  "Some wish to live within the sound of a chapel bell, I want to run a rescue shop within a yard of Hell. -C.T. Studd";
}

void Random_Quote_Server::EstablishSignalsAndSlots()
{

}

int Random_Quote_Server::GetRandom(int low, int high) const
{
    return qrand() % (high - low + 1) + low;
}
