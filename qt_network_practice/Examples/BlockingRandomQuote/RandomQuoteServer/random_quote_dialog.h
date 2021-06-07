#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>

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
};
#endif // DIALOG_H
