#ifndef DOWNLOADPROGRESSDIALOG_H
#define DOWNLOADPROGRESSDIALOG_H

#include <QObject>
#include <QWidget>
#include <QUrl>
#include <QProgressDialog>

class DownloadProgressDialog : public QProgressDialog
{
    Q_OBJECT

public:
    explicit DownloadProgressDialog(const QUrl &url, QWidget *parent = nullptr);

signals:

public slots:
    void OnDownloadProgress(qint64 bytes_read, qint64 bytes_total);
};

#endif // DOWNLOADPROGRESSDIALOG_H
