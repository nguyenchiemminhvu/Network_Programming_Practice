#ifndef DOWNLOADPROGRESSDIALOG_H
#define DOWNLOADPROGRESSDIALOG_H

#include <QObject>
#include <QWidget>
#include <QProgressBar>

class DownloadProgressDialog : public QProgressBar
{
    Q_OBJECT

public:
    explicit DownloadProgressDialog(QWidget *parent = nullptr);

signals:

public slots:
    void OnNetworkReply(qint64 bytes_read, qint64 bytes_total);
};

#endif // DOWNLOADPROGRESSDIALOG_H
