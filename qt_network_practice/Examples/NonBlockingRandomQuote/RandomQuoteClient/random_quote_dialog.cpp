#include "random_quote_dialog.h"
#include "ui_random_quote_dialog.h"

Random_Quote_Dialog::Random_Quote_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Random_Quote_Dialog)
{
    ui->setupUi(this);

    InitUI();
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

void Random_Quote_Dialog::EstablishSignalsAndSlots()
{
    connect(m_leHost, &QLineEdit::textChanged, this, &Random_Quote_Dialog::ReadyToSendRequest);
    connect(m_lePort, &QLineEdit::textChanged, this, &Random_Quote_Dialog::ReadyToSendRequest);
    connect(m_pbQuit, &QPushButton::clicked, this, &Random_Quote_Dialog::close);
    connect(m_pbRequest, &QPushButton::clicked, this, &Random_Quote_Dialog::SendRequest);
}

