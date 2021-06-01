#ifndef DTLS_DIALOG_H
#define DTLS_DIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui { class DTLS_Dialog; }
QT_END_NAMESPACE

class DTLS_Dialog : public QDialog
{
    Q_OBJECT

public:
    DTLS_Dialog(QWidget *parent = nullptr);
    ~DTLS_Dialog();

private:
    Ui::DTLS_Dialog *ui;
};
#endif // DTLS_DIALOG_H
