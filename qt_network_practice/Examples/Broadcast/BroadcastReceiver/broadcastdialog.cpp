#include "broadcastdialog.h"
#include "ui_broadcastdialog.h"

BroadcastDialog::BroadcastDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BroadcastDialog)
{
    ui->setupUi(this);

    InitUI();
    InitSockets();
    EstablishSignalsAndSlots();
}

BroadcastDialog::~BroadcastDialog()
{
    delete ui;
}

void BroadcastDialog::ProcessPendingDatagrams()
{
    QByteArray datagram;

    while (m_broadcast_listener->hasPendingDatagrams())
    {
        datagram.resize(m_broadcast_listener->pendingDatagramSize());
        m_broadcast_listener->readDatagram(datagram.data(), datagram.size());
        m_lStatus->setText(tr("Received datagram: %1").arg(datagram.constData()));
    }
}

void BroadcastDialog::InitUI()
{
    m_lStatus = new QLabel(tr("Listening..."));
    m_pbQuit = new QPushButton(tr("Quit"));

    QGridLayout *main_layout = new QGridLayout();
    main_layout->addWidget(m_lStatus, 0, 0, 1, 3);
    main_layout->addWidget(m_pbQuit, 1, 2, 1, 1);
    this->setLayout(main_layout);
}

void BroadcastDialog::InitSockets()
{
    m_broadcast_listener = new QUdpSocket(this);
    m_broadcast_listener->bind(45454, QUdpSocket::BindFlag::ShareAddress);
}

void BroadcastDialog::EstablishSignalsAndSlots()
{
    connect(m_pbQuit, &QPushButton::clicked, this, &BroadcastDialog::close);
    connect(m_broadcast_listener, &QUdpSocket::readyRead, this, &BroadcastDialog::ProcessPendingDatagrams);
}
