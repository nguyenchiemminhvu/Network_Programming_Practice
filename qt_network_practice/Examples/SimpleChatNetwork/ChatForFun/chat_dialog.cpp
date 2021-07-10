#include "chat_dialog.h"
#include "ui_chat_dialog.h"

Chat_Dialog::Chat_Dialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Chat_Dialog)
{
    ui->setupUi(this);

    InitUI();
    InitChatUser();
    EstablishSignalsAndSlots();
}

Chat_Dialog::~Chat_Dialog()
{
    delete ui;
}

void Chat_Dialog::OnNewParticipant(const QString &nickname)
{
    if (nickname.isEmpty())
        return;

    this->ui->m_lwUsers->addItem(nickname);

    QColor default_color = this->ui->m_teChatBox->textColor();
    this->ui->m_teChatBox->setTextColor(Qt::gray);
    this->ui->m_teChatBox->append(tr("* %1 has joined").arg(nickname));
    this->ui->m_teChatBox->setTextColor(default_color);
}

void Chat_Dialog::OnParticipantLeft(const QString &nickname)
{
    if (nickname.isEmpty())
        return;

    QList<QListWidgetItem *> connected_users = this->ui->m_lwUsers->findItems(nickname, Qt::MatchExactly);
    if (connected_users.isEmpty())
        return;

    delete connected_users.at(0);

    QColor default_color = this->ui->m_teChatBox->textColor();
    this->ui->m_teChatBox->setTextColor(Qt::gray);
    this->ui->m_teChatBox->append(tr("* %1 has left").arg(nickname));
    this->ui->m_teChatBox->setTextColor(default_color);
}

void Chat_Dialog::OnNewMessage(const QString &from, const QString &message)
{
    if (from.isEmpty() || message.isEmpty())
        return;

    QTextCursor cursor(this->ui->m_teChatBox->textCursor());
    cursor.movePosition(QTextCursor::End);

    QTextTable * table = cursor.insertTable(1, 2, m_tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText("<" + from + ">");
    table->cellAt(0, 1).firstCursorPosition().insertText(message);

    QScrollBar * bar = this->ui->m_teChatBox->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Chat_Dialog::OnReturnPressed()
{
    QString message = this->ui->m_leMessage->text();
    if (message.isEmpty())
        return;

    OnNewMessage(m_nickname, message);
    this->ui->m_leMessage->clear();
}

void Chat_Dialog::InitUI()
{
    this->setFixedSize(this->window()->width(), this->window()->height());
    this->ui->m_teChatBox->setReadOnly(true);
    this->ui->m_teChatBox->setFocusPolicy(Qt::NoFocus);
    this->ui->m_lwUsers->setFocusPolicy(Qt::NoFocus);
    this->ui->m_leMessage->setFocusPolicy(Qt::StrongFocus);

    m_tableFormat.setBorder(0);
}

void Chat_Dialog::InitChatUser()
{
    m_user.setParent(this);
    m_nickname = m_user.EnterNickname();
    OnNewParticipant(m_nickname);
}

void Chat_Dialog::EstablishSignalsAndSlots()
{
    connect(this->ui->m_leMessage, &QLineEdit::returnPressed, this, &Chat_Dialog::OnReturnPressed);
    connect(&m_user, &ChatUser::JoinChat, this, &Chat_Dialog::OnNewParticipant);
    connect(&m_user, &ChatUser::LeftChat, this, &Chat_Dialog::OnParticipantLeft);
    connect(&m_user, &ChatUser::SendMessage, this, &Chat_Dialog::OnNewMessage);
}

