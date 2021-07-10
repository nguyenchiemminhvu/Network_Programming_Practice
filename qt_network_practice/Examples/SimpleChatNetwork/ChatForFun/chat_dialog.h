#ifndef CHAT_DIALOG_H
#define CHAT_DIALOG_H

#include <QDialog>
#include <QString>
#include <QDebug>
#include <QGridLayout>
#include <QTextTable>
#include <QTextTableFormat>
#include <QTextCursor>
#include <QScrollBar>
#include <QColor>
#include <QList>
#include <QListWidgetItem>

#include "chat_user.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Chat_Dialog; }
QT_END_NAMESPACE

class Chat_Dialog : public QDialog
{
    Q_OBJECT

public:
    Chat_Dialog(QWidget *parent = nullptr);
    ~Chat_Dialog();

public slots:
    void OnNewParticipant(const QString &nickname);
    void OnParticipantLeft(const QString &nickname);
    void OnNewMessage(const QString &from, const QString &message);
    void OnReturnPressed();

private:
    void InitUI();
    void InitChatUser();
    void EstablishSignalsAndSlots();

private:
    Ui::Chat_Dialog *ui;

private:
    QString m_nickname;
    QTextTableFormat m_tableFormat;

    ChatUser m_user;
};
#endif // CHAT_DIALOG_H
