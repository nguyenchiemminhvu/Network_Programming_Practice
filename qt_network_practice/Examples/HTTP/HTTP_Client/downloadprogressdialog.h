#ifndef DOWNLOADPROGRESSDIALOG_H
#define DOWNLOADPROGRESSDIALOG_H

#include <QObject>
#include <QProgressBar>

class DownloadProgressDialog : public QProgressBar
{
    Q_OBJECT

public:
    explicit DownloadProgressDialog(QObject *parent = nullptr);

signals:

public slots:
    void OnNetworkReply(qint64 bytes_read, qint64 bytes_total);
};

#endif // DOWNLOADPROGRESSDIALOG_H
