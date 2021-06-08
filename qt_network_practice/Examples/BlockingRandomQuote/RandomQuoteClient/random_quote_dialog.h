#ifndef RANDOM_QUOTE_DIALOG_H
#define RANDOM_QUOTE_DIALOG_H

#include <QDialog>
#include <QWidget>
#include <QString>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QAction>
#include <QThread>
#include <QTcpSocket>
#include <QNetworkInterface>

#include "randomquoteworker.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Random_Quote_Dialog; }
QT_END_NAMESPACE

class Random_Quote_Dialog : public QDialog
{
    Q_OBJECT

public:
    Random_Quote_Dialog(QWidget *parent = nullptr);
    ~Random_Quote_Dialog();

    void InitUI();

public slots:
    void ReadyToSendRequest();
    void SendRequestToServer();
    void DisplayQuote(const QString &quote);

private:
    Ui::Random_Quote_Dialog *ui;

private:
    QString m_sQuoteToDisplay;

    QLabel *m_lHost;
    QLabel *m_lPort;
    QLineEdit *m_leHost = NULL;
    QLineEdit *m_lePort = NULL;
    QPushButton *m_pbQuoteRequest;
    QPushButton *m_pbQuit;
    QPlainTextEdit *m_pteQuote;

    RandomQuoteWorker *m_worker = NULL;
    QThread *m_thread = NULL;
};
#endif // RANDOM_QUOTE_DIALOG_H
