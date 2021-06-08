#include "random_quote_dialog.h"
#include "ui_random_quote_dialog.h"

Random_Quote_Dialog::Random_Quote_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Random_Quote_Dialog)
{
    ui->setupUi(this);


}

Random_Quote_Dialog::~Random_Quote_Dialog()
{
    delete ui;
}

void Random_Quote_Dialog::InitUI()
{

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

