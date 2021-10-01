#ifndef ENTRY_H
#define ENTRY_H

#include <QDialog>
#include <QMessageBox>
#include <QObject>

namespace Ui {
class Entry;
}

class Entry : public QDialog
{
    Q_OBJECT

public:
    explicit Entry(QWidget *parent = nullptr);
    ~Entry();

private slots:
    void on_forget_pswd_clicked(); //Забыли пароль
    void on_enter_button_clicked(); //Вход
signals:
    void close_signal();
private:
    Ui::Entry *ui;
};

#endif // ENTRY_H
