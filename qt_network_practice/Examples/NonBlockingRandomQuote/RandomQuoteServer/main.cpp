#include "random_quote_dialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Random_Quote_Dialog w;
    w.show();

    return a.exec();
}

/***
 * m_quotes    <<  "We must be global Christians with a global vision because our God is a global God. -John Stott"
                <<  "God’s work done in God’s way will never lack God’s supplies. -J. Hudson Taylor"
                <<  "If Jesus Christ be God and died for me, then no sacrifice can be too great for me to make for Him. -C.T. Studd"
                <<  "Christians don’t tell lies they just go to church and sing them -A.W. Tozer"
                <<  "We talk of the second coming, half the world has never heard of the first. -Oswald J. Smith"
                <<  "Today Christians spend more money on dog food then missions -Leonard Ravenhill"
                <<  "He must increase, but I must decrease. -John the Baptist"
                <<  "Here am I. Send me. -Isaiah"
                <<  "Some wish to live within the sound of a chapel bell, I want to run a rescue shop within a yard of Hell. -C.T. Studd";
 *
 *
 *
 *
 *
 *
*/
