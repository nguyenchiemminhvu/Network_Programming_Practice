#include "random_quote_dialog.h"
#include "ui_random_quote_dialog.h"

Random_Quote_Dialog::Random_Quote_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Random_Quote_Dialog)
{
    ui->setupUi(this);

    InitServer();
    InitUI();
    EstablishSignalsAndSlots();
}

Random_Quote_Dialog::~Random_Quote_Dialog()
{
    delete ui;
}

void Random_Quote_Dialog::InitUI()
{
    m_lHost = new QLabel(tr("Server IP: %1").arg(m_server_ip));
    m_lPort = new QLabel(tr("Server Port: %1").arg(m_server_port));

    m_pbQuit = new QPushButton(tr("Quit"));

    QGridLayout *main_layout = new QGridLayout();
    main_layout->addWidget(m_lHost, 0, 0, 1, 3);
    main_layout->addWidget(m_lPort, 1, 0, 1, 3);
    main_layout->addWidget(m_pbQuit, 2, 2, 1, 1);

    this->setLayout(main_layout);
}

void Random_Quote_Dialog::InitServer()
{
    QList<QHostAddress> list_ip = QNetworkInterface::allAddresses();
    for (int i = 0; i < list_ip.size(); i++)
    {
        if (list_ip[i] != QHostAddress::LocalHost && list_ip[i].toIPv4Address())
        {
            m_server_ip = list_ip[i].toString();
            break;
        }
    }
    if (m_server_ip.isEmpty())
    {
        m_server_ip = QHostAddress(QHostAddress::LocalHost).toString();
    }

    m_server.listen(QHostAddress(m_server_ip));
    if (!m_server.isListening())
    {
        qDebug() << "Server is not listening";
        this->close();
        return;
    }
    m_server_port = m_server.serverPort();
}

void Random_Quote_Dialog::EstablishSignalsAndSlots()
{
    connect(m_pbQuit, &QPushButton::clicked, this, &Random_Quote_Dialog::close);
}

