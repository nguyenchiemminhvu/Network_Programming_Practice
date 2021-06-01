#include "chat_dialog.h"
#include "ui_chat_dialog.h"

Chat_Dialog::Chat_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Chat_Dialog)
{
    ui->setupUi(this);
}

Chat_Dialog::~Chat_Dialog()
{
    delete ui;
}

