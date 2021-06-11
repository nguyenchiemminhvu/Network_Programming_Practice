#ifndef BROADCASTDIALOG_H
#define BROADCASTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QTimer>
#include <QUdpSocket>
#include <QByteArray>

QT_BEGIN_NAMESPACE
namespace Ui { class BroadcastDialog; }
QT_END_NAMESPACE

class BroadcastDialog : public QDialog
{
    Q_OBJECT

public:
    BroadcastDialog(QWidget *parent = nullptr);
    ~BroadcastDialog();

signals:

public slots:
    void StartBroadcasting();
    void BoardcastDatagram();

private:
    void InitUI();
    void InitSockets();
    void EstablishSignalsAndSlots();

private:
    Ui::BroadcastDialog *ui;

private:
    QTimer m_timer;
    int m_counter;

    QLabel *m_lStatus;
    QPushButton *m_pbSend;
    QPushButton *m_pbQuit;

    QUdpSocket *m_udp_socket;
};
#endif // BROADCASTDIALOG_H
