#include "broadcastdialog.h"
#include "ui_broadcastdialog.h"

BroadcastDialog::BroadcastDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BroadcastDialog)
{
    ui->setupUi(this);
}

BroadcastDialog::~BroadcastDialog()
{
    delete ui;
}

