#include "downloadprogressdialog.h"

DownloadProgressDialog::DownloadProgressDialog(QWidget *parent)
    : QProgressBar(parent)
{
    this->setWindowTitle(tr("Download Progress"));
    this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setMinimum(0);
    this->setValue(0);
}

void DownloadProgressDialog::OnNetworkReply(qint64 bytes_read, qint64 bytes_total)
{
    this->setMaximum(bytes_total);
    this->setValue(bytes_read);
}
