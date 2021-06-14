#include "http_request_dialog.h"
#include "ui_http_request_dialog.h"
#include "ui_authentication_dialog.h"

HTTP_Request_Dialog::HTTP_Request_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HTTP_Request_Dialog)
{
    ui->setupUi(this);

    InitUI();
    EstablishSignalsAndSlots();
}

HTTP_Request_Dialog::~HTTP_Request_Dialog()
{
    delete ui;
}

void HTTP_Request_Dialog::InitUI()
{

}

void HTTP_Request_Dialog::EstablishSignalsAndSlots()
{

}

