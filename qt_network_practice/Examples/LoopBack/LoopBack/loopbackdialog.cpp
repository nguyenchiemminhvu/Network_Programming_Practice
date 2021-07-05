#include "loopbackdialog.h"

static int BytesWritten = 0;
static int BytesToWrite;
static int BytesReceived = 0;
static const int BytesTotal = 50 * 1024 * 1024; // 50 MB
static const int PayloadSize = 64 * 1024; // 64 KB

LoopBackDialog::LoopBackDialog(QWidget *parent)
    : QDialog(parent)
{
    InitUI();
    EstablishSignalsAndSlots();
}

LoopBackDialog::~LoopBackDialog()
{

}

void LoopBackDialog::StartLoopback()
{
    m_pbStart->setEnabled(false);

    while (!m_server.isListening() && !m_server.listen())
    {
        QMessageBox::StandardButton ret = QMessageBox::information(this, tr("Loopback"), tr("Unable to start server"), QMessageBox::Retry | QMessageBox::Cancel);
        if (ret == QMessageBox::Cancel)
        {
            return;
        }
    }

    m_lServerStatus->setText(tr("Listening"));
    m_lClientStatus->setText(tr("Connecting"));
    m_client.connectToHost(QHostAddress::LocalHost, m_server.serverPort());
}

void LoopBackDialog::OnNewConnection()
{
    m_connection = m_server.nextPendingConnection();
    if (!m_connection)
    {
        return;
    }

    connect(m_connection, &QTcpSocket::readyRead, this, &LoopBackDialog::OnReadyRead);
    connect(m_connection, &QTcpSocket::disconnected, m_connection, &QTcpSocket::deleteLater);

    m_lServerStatus->setText(tr("Accepted new connection"));
}

void LoopBackDialog::OnClientConnected()
{
    m_lClientStatus->setText(tr("Connected"));

    qint64 rc = m_client.write(QByteArray(PayloadSize, ' '));
    BytesToWrite = BytesTotal - rc;
}

void LoopBackDialog::OnClientProgress(qint64 bytes_written)
{
    BytesWritten += bytes_written;
    if (BytesToWrite > 0)
    {
        qint64 rc = m_client.write(QByteArray(qMin(BytesToWrite, PayloadSize), ' '));
        BytesToWrite -= rc;
    }

    m_prClient->setMaximum(BytesTotal);
    m_prClient->setValue(BytesWritten);
    m_lClientStatus->setText(tr("Sent: %1 (MB)").arg(BytesWritten / (1024 * 1024)));
}

void LoopBackDialog::OnReadyRead()
{
    BytesReceived += m_connection->bytesAvailable();
    m_connection->readAll();

    m_prServer->setMaximum(BytesTotal);
    m_prServer->setValue(BytesReceived);
    m_lServerStatus->setText(tr("Recv: %1 (MB)").arg(BytesReceived / (1024 * 1024)));

    if (BytesReceived >= BytesTotal)
    {
        m_connection->close();
        m_server.close();
        Reset();
    }
}

void LoopBackDialog::Reset()
{
    m_pbStart->setEnabled(true);

    BytesWritten = 0;
    BytesReceived = 0;
}

void LoopBackDialog::InitUI()
{
    m_prServer = new QProgressBar();
    m_prClient = new QProgressBar();
    m_lServerStatus = new QLabel(tr("Server ready!"));
    m_lClientStatus = new QLabel(tr("Client ready!"));
    m_pbStart = new QPushButton(tr("Start"));
    m_pbQuit = new QPushButton(tr("Quit"));

    QGridLayout * main_layout = new QGridLayout();
    main_layout->addWidget(m_prServer, 0, 0, 1, 4);
    main_layout->addWidget(m_lServerStatus, 1, 0, 1, 4);
    main_layout->addWidget(m_prClient, 2, 0, 1, 4);
    main_layout->addWidget(m_lClientStatus, 3, 0, 1, 4);
    main_layout->addWidget(m_pbStart, 4, 2, 1, 1);
    main_layout->addWidget(m_pbQuit, 4, 3, 1, 1);
    this->setLayout(main_layout);
}

void LoopBackDialog::EstablishSignalsAndSlots()
{
    connect(m_pbQuit, &QPushButton::clicked, this, &LoopBackDialog::close);
    connect(m_pbStart, &QPushButton::clicked, this, &LoopBackDialog::StartLoopback);
    connect(&m_server, &QTcpServer::newConnection, this, &LoopBackDialog::OnNewConnection);
    connect(&m_client, &QTcpSocket::connected, this, &LoopBackDialog::OnClientConnected);
    connect(&m_client, &QTcpSocket::bytesWritten, this, &LoopBackDialog::OnClientProgress);
}

