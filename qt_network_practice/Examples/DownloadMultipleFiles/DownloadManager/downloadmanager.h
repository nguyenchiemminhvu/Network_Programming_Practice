#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrl>
#include <QQueue>
#include <QFile>
#include <QTimer>
#include <QDebug>

class DownloadManager : public QObject
{
    Q_OBJECT

public:
    explicit DownloadManager(QObject *parent = nullptr);

    void Append(const QStringList &urls);
    void Append(const QUrl &url);

signals:
    void Finished();

public slots:
    void StartNextDownload();
    void OnDownloadProgress(qint64 bytes_recv, qint64 bytes_total);
    void OnDownloadFinished();
    void OnReadyRead();

private:
    bool IsHttpRedirect();

private:
    QNetworkAccessManager m_net_manager;
    QNetworkReply *m_current_reply;
    QQueue<QUrl> m_download_queue;
    QFile m_output;

    int downloaded = 0;
    int to_download = 0;
};

#endif // DOWNLOADMANAGER_H
