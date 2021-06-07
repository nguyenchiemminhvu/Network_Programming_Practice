#include "random_quote_dialog.h"
#include "ui_random_quote_dialog.h"

Random_Quote_Dialog::Random_Quote_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Random_Quote_Dialog)
{
    ui->setupUi(this);

    InitUI();
    InitQuotes();
    InitRandomQuoteServer();
}

Random_Quote_Dialog::~Random_Quote_Dialog()
{
    delete ui;
}

void Random_Quote_Dialog::OnClientConnection()
{

}

void Random_Quote_Dialog::InitUI()
{
    this->m_status = new QLabel;
    this->m_status->setTextInteractionFlags(Qt::TextInteractionFlag::TextBrowserInteraction);

    QPushButton *quit_button = new QPushButton(tr("Quit"));
    quit_button->setAutoDefault(false);
    connect(quit_button, &QPushButton::clicked, this, &QDialog::close);

    QHBoxLayout *button_layout = new QHBoxLayout();
    button_layout->addStretch(8);
    button_layout->addWidget(quit_button);
    button_layout->addStretch(1);

    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addWidget(this->m_status);
    main_layout->addLayout(button_layout);

    this->setWindowTitle(QGuiApplication::applicationDisplayName());
}

void Random_Quote_Dialog::InitQuotes()
{

}

void Random_Quote_Dialog::InitRandomQuoteServer()
{

}

