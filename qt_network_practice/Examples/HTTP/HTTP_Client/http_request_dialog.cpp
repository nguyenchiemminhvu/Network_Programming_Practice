#include "http_request_dialog.h"
#include "ui_http_request_dialog.h"

HTTP_Request_Dialog::HTTP_Request_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HTTP_Request_Dialog)
{
    ui->setupUi(this);


}

HTTP_Request_Dialog::~HTTP_Request_Dialog()
{
    delete ui;
}

