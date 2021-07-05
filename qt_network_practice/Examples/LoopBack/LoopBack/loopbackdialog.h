#ifndef LOOPBACKDIALOG_H
#define LOOPBACKDIALOG_H

#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>

#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QMessageBox>
#include <QGridLayout>

#include <QTcpServer>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QByteArray>

class LoopBackDialog : public QDialog
{
    Q_OBJECT

public:
    LoopBackDialog(QWidget *parent = nullptr);
    ~LoopBackDialog();

signals:

public slots:
    void StartLoopback();
    void OnNewConnection();
    void OnClientConnected();
    void OnClientProgress(qint64 bytes_written);
    void OnReadyRead();
    void Reset();

private:
    void InitUI();
    void EstablishSignalsAndSlots();

private:
    QTcpServer m_server;
    QTcpSocket m_client;
    QTcpSocket * m_connection = nullptr;

    QProgressBar * m_prClient;
    QProgressBar * m_prServer;
    QLabel * m_lClientStatus;
    QLabel * m_lServerStatus;
    QPushButton * m_pbStart;
    QPushButton * m_pbQuit;
};
#endif // LOOPBACKDIALOG_H
