#include "downloadprogressdialog.h"

DownloadProgressDialog::DownloadProgressDialog(const QUrl &url, QWidget *parent)
    : QProgressDialog(parent)
{
    this->setWindowTitle(tr("Download Progress"));
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setLabelText(tr("Downloading: %1.").arg(url.toDisplayString()));
    this->setMinimum(0);
    this->setValue(0);
    this->setMinimumDuration(0);
    this->setMinimumSize(400, 75);
}

void DownloadProgressDialog::OnDownloadProgress(qint64 bytes_read, qint64 bytes_total)
{
    this->setMaximum(bytes_total);
    this->setValue(bytes_read);
}
