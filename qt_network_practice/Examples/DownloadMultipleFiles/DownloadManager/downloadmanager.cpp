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
    m_to_download++;

    if (should_start_download && !m_hasStarted)
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
        m_hasStarted = false;
        emit Finished();
    }
}

void DownloadManager::StartNextDownload()
{
    m_hasStarted = true;

    QUrl url = m_download_queue.dequeue();
    QString file_name = FileNameFromUrl(url);
    m_output.setFileName(file_name);
    if (!m_output.open(QIODevice::WriteOnly))
    {
        StartNextDownload();
        return;
    }

    QNetworkRequest request(url);
    m_current_reply = m_net_manager.get(request);
    connect(m_current_reply, &QNetworkReply::downloadProgress, this, &DownloadManager::OnDownloadProgress);
    connect(m_current_reply, &QNetworkReply::finished, this, &DownloadManager::OnDownloadFinished);
    connect(m_current_reply, &QNetworkReply::readyRead, this, &DownloadManager::OnReadyRead);
    connect(m_current_reply, &QNetworkReply::finished, m_current_reply, &QNetworkReply::deleteLater);
}

void DownloadManager::OnDownloadProgress(qint64 bytes_recv, qint64 bytes_total)
{
    qDebug() << QString::number(bytes_recv) + " / " + QString::number(bytes_total);
}

void DownloadManager::OnDownloadFinished()
{
    if (m_current_reply->error())
    {
        qDebug() << m_current_reply->errorString();
        m_output.close();
        m_output.remove();
        return;
    }

    if (IsHttpRedirect())
    {
        m_output.close();
        m_output.remove();
        return;
    }

    m_downloaded++;
    StartNextDownload();
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

QString DownloadManager::FileNameFromUrl(const QUrl &url)
{
    QString path = url.path();
    QString file_name = QFileInfo(path).fileName();

    if (file_name.isEmpty())
    {
        file_name = "download";
    }

    if (QFile::exists(file_name))
    {
        int i = 0;
        while (true)
        {
            int last_dot = file_name.lastIndexOf('.');
            QString file_name_without_ext = file_name.left(last_dot);
            QString ext = file_name.right(file_name.length() - last_dot);
            QString next_candidate = file_name_without_ext + '_' + QString::number(i) + ext;
            if (!QFile::exists(next_candidate))
            {
                file_name = next_candidate;
                break;
            }
            i++;
        }
    }

    return file_name;
}
