#include "chat_dialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Chat_Dialog w;
    w.show();

    return a.exec();
}
