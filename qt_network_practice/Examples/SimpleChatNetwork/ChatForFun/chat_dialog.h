#ifndef CHAT_DIALOG_H
#define CHAT_DIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class Chat_Dialog; }
QT_END_NAMESPACE

class Chat_Dialog : public QDialog
{
    Q_OBJECT

public:
    Chat_Dialog(QWidget *parent = nullptr);
    ~Chat_Dialog();

private:
    Ui::Chat_Dialog *ui;
};
#endif // CHAT_DIALOG_H
