#ifndef HTTP_REQUEST_DIALOG_H
#define HTTP_REQUEST_DIALOG_H

#include "downloadprogressdialog.h"

#include <QDialog>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslError>
#include <QAuthenticator>
#include <QFile>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui { class HTTP_Request_Dialog; }
QT_END_NAMESPACE

class HTTP_Request_Dialog : public QDialog
{
    Q_OBJECT

public:
    HTTP_Request_Dialog(QWidget *parent = nullptr);
    ~HTTP_Request_Dialog();

signals:

private slots:


private:
    void InitUI();
    void EstablishSignalsAndSlots();

private:
    Ui::HTTP_Request_Dialog *ui;

private:
    QUrl m_url;
    QNetworkAccessManager m_access_manager;
    QNetworkReply *m_reply;
    std::unique_ptr<QFile> m_file;
    bool m_bRequestAborted;
};
#endif // HTTP_REQUEST_DIALOG_H
