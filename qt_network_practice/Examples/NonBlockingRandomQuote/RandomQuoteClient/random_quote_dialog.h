#ifndef RANDOM_QUOTE_DIALOG_H
#define RANDOM_QUOTE_DIALOG_H

#include <QDialog>
#include <QString>
#include <QThread>
#include <QTcpSocket>
#include <QDataStream>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QPlainTextEdit>
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

public slots:
    void SendRequest();

private:
    void InitUI();
    void EstablishSignalsAndSlots();

private:
    Ui::Random_Quote_Dialog *ui;

private:
    QTcpSocket *m_soc_client;

    QLabel *m_lHost;
    QLabel *m_lPort;
    QLineEdit *m_leHost;
    QLineEdit *m_lePort;
    QPushButton *m_pbRequest;
    QPushButton *m_pbQuit;
    QPlainTextEdit *m_pteQuote;
};
#endif // RANDOM_QUOTE_DIALOG_H
