#ifndef LOOPBACKDIALOG_H
#define LOOPBACKDIALOG_H

#include <QDialog>
#include <QTcpServer>
#include <QTcpSocket>

#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QGridLayout>

QT_BEGIN_NAMESPACE
namespace Ui { class LoopBackDialog; }
QT_END_NAMESPACE

class LoopBackDialog : public QDialog
{
    Q_OBJECT

public:
    LoopBackDialog(QWidget *parent = nullptr);
    ~LoopBackDialog();

private:
    Ui::LoopBackDialog *ui;
};
#endif // LOOPBACKDIALOG_H
