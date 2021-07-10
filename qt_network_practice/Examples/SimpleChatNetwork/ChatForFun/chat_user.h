#ifndef CHATUSER_H
#define CHATUSER_H

#include <QObject>
#include <QInputDialog>

class ChatUser : public QObject
{
    Q_OBJECT
public:
    explicit ChatUser(QObject *parent = nullptr);

    QString EnterNickname();

signals:
    void JoinChat(const QString &nickname);
    void LeftChat(const QString &nickname);
    void SendMessage(const QString &nickname, const QString &message);

public slots:


private:

};

#endif // CHATUSER_H
