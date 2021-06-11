#ifndef RANDOM_QUOTE_DIALOG_H
#define RANDOM_QUOTE_DIALOG_H

#include <QDialog>
#include <QString>
#include <QTimer>
#include <QThread>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QDataStream>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QGridLayout>
#include <QMessageBox>
#include <QDebug>

QT_BEGIN_NAMESPACE
namespace Ui { class Random_Quote_Dialog; }
QT_END_NAMESPACE

class Random_Quote_Dialog : public QDialog
{
    Q_OBJECT

public:
    Random_Quote_Dialog(QWidget *parent = nullptr);
    ~Random_Quote_Dialog();

signals:
    void UpdateUI();

public slots:
    void ReadyToSendRequest();
    void SendRequest();
    void OnQuoteAvailable();
    void OnErrorHappenned(QAbstractSocket::SocketError err);

private:
    void InitUI();
    void InitSocket();
    void EstablishSignalsAndSlots();

private:
    Ui::Random_Quote_Dialog *ui;

private:
    QString m_quote_to_display;
    QTcpSocket *m_soc_client;
    QDataStream m_stream_client;

    QLabel *m_lHost;
    QLabel *m_lPort;
    QLineEdit *m_leHost;
    QLineEdit *m_lePort;
    QPushButton *m_pbRequest;
    QPushButton *m_pbQuit;
    QPlainTextEdit *m_pteQuote;
};
#endif // RANDOM_QUOTE_DIALOG_H
