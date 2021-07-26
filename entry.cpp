#include "entry.h"
#include "ui_entry.h"

Entry::Entry(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Entry)
{
    ui->setupUi(this);
}

Entry::~Entry()
{
    delete ui;
}

void Entry::on_forget_pswd_clicked()
{
    QMessageBox msg;
    msg.setWindowTitle("Забыли пароль?");
    msg.setText("Обратитесь к начальнику службы!");
    msg.exec();
}

void Entry::on_enter_button_clicked()
{
    emit close_signal();
    Entry::close();
    emit close_signal();

}
