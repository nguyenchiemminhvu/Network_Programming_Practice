#include "chat_user.h"
#include "chat_dialog.h"

ChatUser::ChatUser(QObject *parent) : QObject(parent)
{

}

QString ChatUser::EnterNickname()
{
    QString nick;
    bool is_ok_pressed = false;
    do
    {
        nick = QInputDialog::getText((Chat_Dialog *)this->parent(), "Chat For Fun", "Enter nickname:", QLineEdit::Normal, QString(), &is_ok_pressed);
        if (!is_ok_pressed)
        {
            exit(0);
        }
    } while (nick.isEmpty());

    return nick;
}
