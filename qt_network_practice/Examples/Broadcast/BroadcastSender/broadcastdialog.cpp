#include "broadcastdialog.h"
#include "ui_broadcastdialog.h"

BroadcastDialog::BroadcastDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BroadcastDialog)
{
    ui->setupUi(this);

    m_counter = 0;

    InitUI();
    InitSockets();
    EstablishSignalsAndSlots();
}

BroadcastDialog::~BroadcastDialog()
{
    delete ui;
}

void BroadcastDialog::StartBroadcasting()
{
    m_pbSend->setEnabled(false);
    m_timer.start(1000);
}

void BroadcastDialog::BoardcastDatagram()
{
    m_counter++;
    m_lStatus->setText(tr("Broadcast datagram %1").arg(m_counter));

    QByteArray datagram = "Broadcast message " + QByteArray::number(m_counter);
    m_udp_socket->writeDatagram(datagram, QHostAddress::Broadcast, 45454);
}

void BroadcastDialog::InitUI()
{
    m_lStatus = new QLabel(tr("Ready to send broadcast on port 45454"));
    m_lStatus->setWordWrap(true);

    m_pbSend = new QPushButton(tr("Start"));
    m_pbQuit = new QPushButton(tr("Quit"));

    QGridLayout *main_layout = new QGridLayout();
    main_layout->addWidget(m_lStatus, 0, 0, 1, 3);
    main_layout->addWidget(m_pbSend, 1, 1, 1, 1);
    main_layout->addWidget(m_pbQuit, 1, 2, 1, 1);
    this->setLayout(main_layout);
}

void BroadcastDialog::InitSockets()
{
    m_udp_socket = new QUdpSocket(this);
}

void BroadcastDialog::EstablishSignalsAndSlots()
{
    connect(m_pbQuit, &QPushButton::clicked, this, &BroadcastDialog::close);
    connect(m_pbSend, &QPushButton::clicked, this, &BroadcastDialog::StartBroadcasting);
    connect(&m_timer, &QTimer::timeout, this, &BroadcastDialog::BoardcastDatagram);
}

