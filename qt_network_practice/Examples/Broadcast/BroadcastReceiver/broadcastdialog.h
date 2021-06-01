#ifndef BROADCASTDIALOG_H
#define BROADCASTDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class BroadcastDialog; }
QT_END_NAMESPACE

class BroadcastDialog : public QDialog
{
    Q_OBJECT

public:
    BroadcastDialog(QWidget *parent = nullptr);
    ~BroadcastDialog();

private:
    Ui::BroadcastDialog *ui;
};
#endif // BROADCASTDIALOG_H
