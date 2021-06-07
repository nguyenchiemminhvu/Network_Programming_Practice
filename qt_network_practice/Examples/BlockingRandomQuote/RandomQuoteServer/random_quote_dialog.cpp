#include "random_quote_dialog.h"
#include "ui_random_quote_dialog.h"

Random_Quote_Dialog::Random_Quote_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Random_Quote_Dialog)
{
    ui->setupUi(this);

    InitUI();
    InitQuotes();
    InitRandom();
    InitRandomQuoteServer();
}

Random_Quote_Dialog::~Random_Quote_Dialog()
{
    delete ui;
}

void Random_Quote_Dialog::OnClientConnection()
{
    int quote_index = RandomInt(0, m_quotes.size() - 1);
    qDebug() << m_quotes[quote_index];
}

void Random_Quote_Dialog::InitUI()
{
    this->m_status = new QLabel;
    this->m_status->setTextInteractionFlags(Qt::TextInteractionFlag::TextBrowserInteraction);

    QPushButton *quit_button = new QPushButton(tr("Quit"));
    quit_button->setAutoDefault(false);
    connect(quit_button, &QPushButton::clicked, this, &QDialog::close);

    QHBoxLayout *button_layout = new QHBoxLayout();
    button_layout->addSpacing(100);
    button_layout->addWidget(quit_button);

    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addWidget(this->m_status);
    main_layout->addLayout(button_layout);

    this->setWindowTitle(QGuiApplication::applicationDisplayName());
}

void Random_Quote_Dialog::InitQuotes()
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

void Random_Quote_Dialog::InitRandom()
{
    QTime cur = QTime::currentTime();
    qsrand((uint)cur.msec());
}

void Random_Quote_Dialog::InitRandomQuoteServer()
{
    m_server = new QTcpServer(this);
    if (!m_server->listen())
    {
        QMessageBox::critical(this, "Random Quote Server", tr("Unable to start server"));
        this->close();
        return;
    }

    connect(m_server, &QTcpServer::newConnection, this, &Random_Quote_Dialog::OnClientConnection);

    QString server_address;
    QList<QHostAddress> server_addresses = QNetworkInterface::allAddresses();
    for (int i = 0; i < server_addresses.size(); i++)
    {
        if (server_addresses[i] != QHostAddress::LocalHost && server_addresses[i].toIPv4Address())
        {
            server_address = server_addresses[i].toString();
            break;
        }
    }

    if (server_address.isEmpty())
    {
        server_address = QHostAddress(QHostAddress::LocalHost).toString();
    }

    this->m_status->setText(tr("The server is running\n\nIP: %1\nPort: %2\n").arg(server_address).arg(m_server->serverPort()));
}

int Random_Quote_Dialog::RandomInt(int low, int high)
{
    return qrand() % (high + 1 - low) + low;
}

