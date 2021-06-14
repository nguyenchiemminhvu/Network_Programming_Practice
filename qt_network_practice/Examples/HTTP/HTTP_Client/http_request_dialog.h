#ifndef HTTP_REQUEST_DIALOG_H
#define HTTP_REQUEST_DIALOG_H

#include "downloadprogressdialog.h"

#include <QDialog>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslError>
#include <QAuthenticator>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>

QT_BEGIN_NAMESPACE
namespace Ui { class HTTP_Request_Dialog; }
QT_END_NAMESPACE

class HTTP_Request_Dialog : public QDialog
{
    Q_OBJECT

public:
    HTTP_Request_Dialog(QWidget *parent = nullptr);
    ~HTTP_Request_Dialog();

private:
    Ui::HTTP_Request_Dialog *ui;
};
#endif // HTTP_REQUEST_DIALOG_H
