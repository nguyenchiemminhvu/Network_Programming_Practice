#include "random_quote_dialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Random_Quote_Dialog w;
    w.show();

    return a.exec();
}
