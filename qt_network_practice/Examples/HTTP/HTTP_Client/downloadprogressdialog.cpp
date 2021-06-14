#include "downloadprogressdialog.h"

DownloadProgressDialog::DownloadProgressDialog(QWidget *parent)
    : QProgressBar(parent)
{

}

void DownloadProgressDialog::OnNetworkReply(qint64 bytes_read, qint64 bytes_total)
{

}
