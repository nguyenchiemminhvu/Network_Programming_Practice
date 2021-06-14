#ifndef HTTP_REQUEST_DIALOG_H
#define HTTP_REQUEST_DIALOG_H

#include "downloadprogressdialog.h"

#include <QDialog>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSslError>
#include <QAuthenticator>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QIODevice>
#include <QStandardPaths>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QMessageBox>

#include <QString>
#include <QList>

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

    void SendRequest(const QUrl &url);

signals:

private slots:
    void UpdateUI();

    void StartDownload();
    void OnDownloadCanceled();
    void OnDownloadFinished();
    void OnReadyRead();

    void OnAuthenticationRequired(QNetworkReply *rep, QAuthenticator *auth);
#ifndef QT_NO_SSL
    void OnSslError(QNetworkReply *rep, QList<QSslError> errors);
#endif

private:
    void InitUI();
    void EstablishSignalsAndSlots();
    std::unique_ptr<QFile> OpenFileToWrite(const QString &file_name);

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
