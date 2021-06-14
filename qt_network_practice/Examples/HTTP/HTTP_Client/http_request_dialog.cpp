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
    m_url = url;
    m_bRequestAborted = false;

    m_reply = m_access_manager.get(QNetworkRequest(url));
    connect(m_reply, &QNetworkReply::finished, this, &HTTP_Request_Dialog::OnDownloadFinished);
    connect(m_reply, &QNetworkReply::readyRead, this, &HTTP_Request_Dialog::OnReadyRead);

    DownloadProgressDialog *download_dialog = new DownloadProgressDialog(url, this);
    connect(download_dialog, &DownloadProgressDialog::canceled, this, &HTTP_Request_Dialog::OnDownloadCanceled);
    connect(m_reply, &QNetworkReply::downloadProgress, download_dialog, &DownloadProgressDialog::OnDownloadProgress);
    connect(m_reply, &QNetworkReply::finished, download_dialog, &DownloadProgressDialog::hide);
    connect(m_reply, &QNetworkReply::finished, download_dialog, &DownloadProgressDialog::deleteLater);
    connect(m_reply, &QNetworkReply::finished, m_reply, &QNetworkReply::deleteLater);

    download_dialog->show();
}

void HTTP_Request_Dialog::UpdateUI()
{
    ui->pb_download->setEnabled(!ui->le_url->text().isEmpty());
}

void HTTP_Request_Dialog::StartDownload()
{
    QString url_spec = ui->le_url->text().trimmed();
    if (url_spec.isEmpty())
    {
        return;
    }

    QUrl url_valid = QUrl::fromUserInput(url_spec);
    if (!url_valid.isValid())
    {
        QMessageBox::information(this, tr("ERROR"), tr("Invalid URL: %1").arg(url_spec));
        return;
    }

    QString file_name = url_valid.fileName();
    if (file_name.isEmpty())
        file_name = ui->le_default_file->text().trimmed();
    if (file_name.isEmpty())
        file_name = "HTTP_Client_downloaded.dat";

    QString download_dir = QDir::cleanPath(ui->le_directory->text().trimmed());
    if (!download_dir.isEmpty() && QFileInfo(download_dir).isDir())
    {
        file_name.prepend(download_dir + '/');
    }

    if (QFile::exists(file_name))
    {
        int rc = QMessageBox::information(this, tr("File exists"), tr("Overwrite?"), QMessageBox::Yes | QMessageBox::No);
        if (rc == QMessageBox::Yes)
        {
            QFile::remove(file_name);
        }
    }

    m_file = this->OpenFileToWrite(file_name);
    if (!m_file)
    {
        return;
    }

    ui->pb_download->setEnabled(false);
    SendRequest(url_valid);
}

void HTTP_Request_Dialog::OnDownloadCanceled()
{
    m_bRequestAborted = true;
    m_reply->abort();
    UpdateUI();
}

void HTTP_Request_Dialog::OnDownloadFinished()
{
    QFileInfo file_info;
    if (m_file)
    {
        file_info.setFile(m_file->fileName());
        m_file->close();
        m_file.reset();
    }

    if (m_bRequestAborted)
    {
        UpdateUI();
        m_reply = nullptr;
        return;
    }

    if (m_reply->error())
    {
        QFile::remove(file_info.absolutePath());
        UpdateUI();
        m_reply = nullptr;
        return;
    }

    const QVariant redirection_attr = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    m_reply = nullptr;

    if (!redirection_attr.isNull())
    {
        const QUrl redirection_url = m_url.resolved(redirection_attr.toUrl());
        int rc = QMessageBox::information(this, tr("Redirect"), tr("Redirect to %1").arg(redirection_url.toString()));
        if (rc == QMessageBox::No)
        {
            QFile::remove(file_info.absolutePath());
            UpdateUI();
            return;
        }
        else
        {
            m_file = this->OpenFileToWrite(file_info.absolutePath());
            if (!m_file)
            {
                UpdateUI();
                return;
            }
            SendRequest(redirection_url);
            return;
        }
    }

    UpdateUI();

    if (ui->cb_launch_file->isChecked())
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(file_info.absoluteFilePath()));
    }
}

void HTTP_Request_Dialog::OnReadyRead()
{
    if (m_file)
    {
        m_file->write(m_reply->readAll());
    }
}

void HTTP_Request_Dialog::OnAuthenticationRequired(QNetworkReply *rep, QAuthenticator *auth)
{
    QDialog authen_dialog;
    Ui::Authentication_Dialog ui;
    ui.setupUi(&authen_dialog);

    ui.l_description->setText(tr("%1 at %2").arg(auth->realm()).arg(m_url.host()));

    if (authen_dialog.exec() == QDialog::Accepted)
    {
        auth->setUser(ui.le_username->text());
        auth->setPassword(ui.le_password->text());
    }
}

#ifndef QT_NO_SSL
void HTTP_Request_Dialog::OnSslError(QNetworkReply *rep, QList<QSslError> errors)
{
    QString errorString;
    for (const QSslError &error : errors)
    {
        if (!errorString.isEmpty())
            errorString += '\n';
        errorString += error.errorString();
    }

    if (QMessageBox::warning(this, tr("SSL Errors"),
                             tr("One or more SSL errors has occurred:\n%1").arg(errorString),
                             QMessageBox::Ignore | QMessageBox::Abort) == QMessageBox::Ignore)
    {
        rep->ignoreSslErrors();
    }
}
#endif

void HTTP_Request_Dialog::InitUI()
{
    QString download_dir = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    if (download_dir.isEmpty() || !QFileInfo(download_dir).isDir())
        download_dir = QDir::currentPath();
    ui->le_directory->setText(QDir::toNativeSeparators(download_dir));

    ui->pb_download->setEnabled(false);
}

void HTTP_Request_Dialog::EstablishSignalsAndSlots()
{
    connect(ui->le_url, &QLineEdit::textChanged, this, &HTTP_Request_Dialog::UpdateUI);
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
