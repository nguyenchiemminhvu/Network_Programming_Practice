#include "http_request_dialog.h"
#include "ui_http_request_dialog.h"
#include "ui_authentication_dialog.h"

HTTP_Request_Dialog::HTTP_Request_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::HTTP_Request_Dialog)
{
    ui->setupUi(this);

    m_reply = NULL;
    m_file = NULL;
    m_bRequestAborted = false;

    InitUI();
    EstablishSignalsAndSlots();
}

HTTP_Request_Dialog::~HTTP_Request_Dialog()
{
    delete ui;
}

void HTTP_Request_Dialog::SendRequest(const QUrl &url)
{

}

void HTTP_Request_Dialog::UpdateUI()
{

}

void HTTP_Request_Dialog::StartDownload()
{

}

void HTTP_Request_Dialog::CancelDownload()
{

}

void HTTP_Request_Dialog::OnDownloadFinished()
{

}

void HTTP_Request_Dialog::OnReadyRead()
{

}

void HTTP_Request_Dialog::OnAuthenticationRequired(QNetworkReply *rep, QAuthenticator *auth)
{

}

#ifndef QT_NO_SSL
void HTTP_Request_Dialog::OnSslError(QNetworkReply *rep, QList<QSslError> errors)
{

}
#endif

void HTTP_Request_Dialog::InitUI()
{
    QString download_dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    if (download_dir.isEmpty() || !QFileInfo(download_dir).isDir())
        download_dir = QDir::currentPath();
    ui->le_directory->setText(QDir::toNativeSeparators(download_dir));
}

void HTTP_Request_Dialog::EstablishSignalsAndSlots()
{
    connect(ui->pb_quit, &QPushButton::clicked, this, &HTTP_Request_Dialog::close);
    connect(ui->pb_download, &QPushButton::clicked, this, &HTTP_Request_Dialog::StartDownload);

    connect(&m_access_manager, &QNetworkAccessManager::authenticationRequired, this, &HTTP_Request_Dialog::OnAuthenticationRequired);
#ifndef QT_NO_SSL
    connect(&m_access_manager, &QNetworkAccessManager::sslErrors, this, &HTTP_Request_Dialog::OnSslError);
#endif
}

std::unique_ptr<QFile> HTTP_Request_Dialog::OpenFileToWrite(const QString &file_name)
{
    std::unique_ptr<QFile> file(new QFile(file_name));
    if (!file->open(QIODevice::OpenModeFlag::WriteOnly))
    {
        QMessageBox::information(this, tr("ERROR"), tr("Unable to open file %1: %2").arg(QDir::toNativeSeparators(file_name).arg(file->errorString())));
        return nullptr;
    }

    return file;
}

