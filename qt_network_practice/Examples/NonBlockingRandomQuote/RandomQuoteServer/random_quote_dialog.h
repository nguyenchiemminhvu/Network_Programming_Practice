#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QList>
#include <QDebug>
#include "random_quote_server.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Random_Quote_Dialog; }
QT_END_NAMESPACE

class Random_Quote_Dialog : public QDialog
{
    Q_OBJECT

public:
    Random_Quote_Dialog(QWidget *parent = nullptr);
    ~Random_Quote_Dialog();

private:
    void InitUI();
    void InitServer();
    void EstablishSignalsAndSlots();

private:
    Ui::Random_Quote_Dialog *ui;

private:
    QLabel *m_lHost;
    QLabel *m_lPort;
    QPushButton *m_pbQuit;

    QString m_server_ip;
    quint16 m_server_port;
    Random_Quote_Server m_server;
};
#endif // DIALOG_H
