#ifndef DIALOG_H
#define DIALOG_H

#include <QDebug>
#include <QDialog>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QNetworkInterface>
#include <QMessageBox>
#include <QPushButton>
#include <QTcpServer>
#include <QString>
#include <QVector>
#include <QTime>

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
    Ui::Random_Quote_Dialog *ui;

    ////////////////////////////////////////////////////
    // User-define section
public slots:
    void OnClientConnection();

private:
    void InitUI();
    void InitQuotes();
    void InitRandom();
    void InitRandomQuoteServer();

    int RandomInt(int low, int high);

private:
    QVector<QString> m_quotes;

    QLabel *m_status = NULL;
    QTcpServer *m_server = NULL;
};
#endif // DIALOG_H
