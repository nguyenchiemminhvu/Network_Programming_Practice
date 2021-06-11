#include "random_quote_dialog.h"
#include "ui_random_quote_dialog.h"

Random_Quote_Dialog::Random_Quote_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Random_Quote_Dialog)
{
    ui->setupUi(this);

    InitUI();
    InitSocket();
    EstablishSignalsAndSlots();
}

Random_Quote_Dialog::~Random_Quote_Dialog()
{
    delete ui;
}

void Random_Quote_Dialog::ReadyToSendRequest()
{
    if (m_leHost->text().isEmpty() || m_lePort->text().isEmpty())
        m_pbRequest->setEnabled(false);
    else
        m_pbRequest->setEnabled(true);
}

void Random_Quote_Dialog::SendRequest()
{
    if (!m_soc_client)
        return;

    m_pbRequest->setEnabled(false);
    m_soc_client->abort();
    m_soc_client->connectToHost(m_leHost->text(), m_lePort->text().toInt());
}

void Random_Quote_Dialog::OnQuoteAvailable()
{
    QString next_quote;

    m_stream_client.startTransaction();
    m_stream_client >> next_quote;

    if (!m_stream_client.commitTransaction())
    {
        return;
    }

    if (m_quote_to_display == next_quote)
    {
        QTimer::singleShot(0, this, &Random_Quote_Dialog::SendRequest);
        return;
    }

    m_quote_to_display = next_quote;
    m_pteQuote->setPlainText(m_quote_to_display);

    emit UpdateUI();
}

void Random_Quote_Dialog::OnErrorHappenned(QAbstractSocket::SocketError err)
{
    switch (err)
    {
    case QAbstractSocket::SocketError::RemoteHostClosedError:
        break;
    case QAbstractSocket::SocketError::HostNotFoundError:
        QMessageBox::information(this, tr("Random Quote Client"), tr("404 Not found"));
        break;
    case QAbstractSocket::SocketError::ConnectionRefusedError:
        QMessageBox::information(this, tr("Random Quote Client"), tr("Connection refused by peer"));
        break;

    default:
        break;
    }
}

void Random_Quote_Dialog::InitUI()
{
    m_lHost = new QLabel(tr("Host: "));
    m_lPort = new QLabel(tr("Port: "));
    m_leHost = new QLineEdit();
    m_lePort = new QLineEdit();
    m_pbRequest = new QPushButton(tr("Request"));
    m_pbQuit = new QPushButton(tr("Quit"));
    m_pteQuote = new QPlainTextEdit();

    QGridLayout *main_layout = new QGridLayout();
    main_layout->addWidget(m_lHost, 0, 0, 1, 1);
    main_layout->addWidget(m_leHost, 0, 1, 1, 4);
    main_layout->addWidget(m_lPort, 1, 0, 1, 1);
    main_layout->addWidget(m_lePort, 1, 1, 1, 4);
    main_layout->addWidget(m_pbRequest, 2, 3, 1, 1);
    main_layout->addWidget(m_pbQuit, 2, 4, 1, 1);
    main_layout->addWidget(m_pteQuote, 3, 0, 2, 5);
    this->setLayout(main_layout);

    m_pbRequest->setEnabled(false);
    m_pteQuote->setEnabled(false);
}

void Random_Quote_Dialog::InitSocket()
{
    m_soc_client = new QTcpSocket();
    m_stream_client.setDevice(m_soc_client);
}

void Random_Quote_Dialog::EstablishSignalsAndSlots()
{
    connect(this, &Random_Quote_Dialog::UpdateUI, this, &Random_Quote_Dialog::ReadyToSendRequest);
    connect(m_leHost, &QLineEdit::textChanged, this, &Random_Quote_Dialog::ReadyToSendRequest);
    connect(m_lePort, &QLineEdit::textChanged, this, &Random_Quote_Dialog::ReadyToSendRequest);
    connect(m_pbQuit, &QPushButton::clicked, this, &Random_Quote_Dialog::close);
    connect(m_pbRequest, &QPushButton::clicked, this, &Random_Quote_Dialog::SendRequest);
    connect(m_soc_client, &QTcpSocket::readyRead, this, &Random_Quote_Dialog::OnQuoteAvailable);
    connect(m_soc_client, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, &Random_Quote_Dialog::OnErrorHappenned);
}

