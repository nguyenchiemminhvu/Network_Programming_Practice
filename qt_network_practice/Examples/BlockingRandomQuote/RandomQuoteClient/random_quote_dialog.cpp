#include "random_quote_dialog.h"
#include "ui_random_quote_dialog.h"

Random_Quote_Dialog::Random_Quote_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Random_Quote_Dialog)
{
    ui->setupUi(this);

    InitUI();
}

Random_Quote_Dialog::~Random_Quote_Dialog()
{
    delete ui;
}

void Random_Quote_Dialog::InitUI()
{
    QString server_address;
    QList<QHostAddress> addresses = QNetworkInterface::allAddresses();
    for (int i = 0; i < addresses.size(); i++)
    {
        if (addresses[i] != QHostAddress::LocalHost && addresses[i].toIPv4Address())
        {
            server_address = addresses[i].toString();
            break;
        }
    }
    if (server_address.isEmpty())
    {
        server_address = QHostAddress(QHostAddress::LocalHost).toString();
    }

    m_lHost = new QLabel(tr("Server name:"));
    m_lPort = new QLabel(tr("Server port:"));

    m_leHost = new QLineEdit(server_address);
    m_lePort = new QLineEdit();
    m_lePort->setValidator(new QIntValidator(1025, 65535, this));

    m_pbQuit = new QPushButton(tr("Quit"));
    m_pbQuoteRequest = new QPushButton(tr("Request"));
    m_pbQuoteRequest->setDefault(true);
    m_pbQuoteRequest->setEnabled(false);

    QDialogButtonBox *button_box = new QDialogButtonBox(Qt::Orientation::Horizontal);
    button_box->addButton(m_pbQuoteRequest, QDialogButtonBox::ButtonRole::ActionRole);
    button_box->addButton(m_pbQuit, QDialogButtonBox::ButtonRole::RejectRole);

    m_pteQuote = new QPlainTextEdit();
    m_pteQuote->setEnabled(false);

    QGridLayout *main_layout = new QGridLayout();
    main_layout->addWidget(m_lHost, 0, 0);
    main_layout->addWidget(m_leHost, 0, 1, 1, 2);
    main_layout->addWidget(m_lPort, 1, 0);
    main_layout->addWidget(m_lePort, 1, 1, 1, 2);
    main_layout->addWidget(button_box, 2, 1, 1, 2);
    main_layout->addWidget(m_pteQuote, 3, 0, 2, 3);
    this->setLayout(main_layout);
}

void Random_Quote_Dialog::ReadyToSendRequest()
{
    if (m_leHost && !m_leHost->text().isEmpty()
     && m_lePort && !m_lePort->text().isEmpty())
    {
        if (m_pbQuoteRequest)
        {
            m_pbQuoteRequest->setEnabled(true);
        }
    }
}

void Random_Quote_Dialog::SendRequestToServer()
{

}

void Random_Quote_Dialog::DisplayQuote(const QString &quote)
{

}

