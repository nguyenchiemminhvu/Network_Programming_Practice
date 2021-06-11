#ifndef BROADCASTDIALOG_H
#define BROADCASTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
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

private slots:
    void ProcessPendingDatagrams();

private:
    void InitUI();
    void InitSockets();
    void EstablishSignalsAndSlots();

private:
    Ui::BroadcastDialog *ui;

private:
    QUdpSocket *m_broadcast_listener;

    QLabel *m_lStatus;
    QPushButton *m_pbQuit;
};
#endif // BROADCASTDIALOG_H
