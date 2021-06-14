#include "loopbackdialog.h"
#include "ui_loopbackdialog.h"

LoopBackDialog::LoopBackDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoopBackDialog)
{
    ui->setupUi(this);
}

LoopBackDialog::~LoopBackDialog()
{
    delete ui;
}

