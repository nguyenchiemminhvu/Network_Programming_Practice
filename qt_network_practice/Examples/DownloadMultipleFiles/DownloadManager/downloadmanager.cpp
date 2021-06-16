#include "downloadmanager.h"

DownloadManager::DownloadManager(QObject *parent)
    : QObject(parent)
{

}

void DownloadManager::Append(const QUrl &url)
{
    bool should_start_download = false;
    if (m_download_queue.isEmpty())
    {
        should_start_download = true;
    }

    m_download_queue.enqueue(url);

    if (should_start_download)
    {
        QTimer::singleShot(0, this, &DownloadManager::StartNextDownload);
    }
}

void DownloadManager::Append(const QStringList &urls)
{
    for (QString url : urls)
    {
        this->Append(QUrl::fromEncoded(url.toLocal8Bit()));
    }

    if (m_download_queue.isEmpty())
    {
        emit Finished();
    }
}

void DownloadManager::StartNextDownload()
{

}

void DownloadManager::OnDownloadProgress(qint64 bytes_recv, qint64 bytes_total)
{

}

void DownloadManager::OnDownloadFinished()
{

}

void DownloadManager::OnReadyRead()
{
    if (m_current_reply)
        m_output.write(m_current_reply->readAll());
}

bool DownloadManager::IsHttpRedirect()
{
    if (m_current_reply)
    {
        int status = m_current_reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toInt();
        return status == 301 || status == 302 || status == 303 || status == 305 || status == 307 || status == 308;
    }
    return false;
}
