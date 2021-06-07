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

}

void Random_Quote_Dialog::InitQuotes()
{

}

void Random_Quote_Dialog::InitRandomQuoteServer()
{

}

