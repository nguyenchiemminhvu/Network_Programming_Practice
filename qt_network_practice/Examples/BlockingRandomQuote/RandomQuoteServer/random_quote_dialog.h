#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTcpServer>
#include <QString>
#include <QVector>

QT_BEGIN_NAMESPACE
class QLabel;
class QTcpServer;
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
    void InitRandomQuoteServer();

private:
    QVector<QString> m_quotes;

    QLabel *status = NULL;
    QTcpServer *server = NULL;
};
#endif // DIALOG_H
