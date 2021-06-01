#include "dtls_dialog.h"
#include "ui_dtls_dialog.h"

DTLS_Dialog::DTLS_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DTLS_Dialog)
{
    ui->setupUi(this);
}

DTLS_Dialog::~DTLS_Dialog()
{
    delete ui;
}

