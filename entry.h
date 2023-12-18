#ifndef ENTRY_H
#define ENTRY_H

#include <QDialog>
#include <QMessageBox>
#include <QObject>
#include <QtSql>

namespace Ui {
class Entry;
}

class Entry : public QDialog
{
    Q_OBJECT

public:
    explicit Entry(QWidget *parent = nullptr);
    ~Entry();
    QString currUser; //Текущий пользователь
    QString currUserRights; //Права текущего пользователя
private slots:
    void on_forget_pswd_clicked(); //Забыли пароль
    void on_enter_button_clicked(); //Вход
    void closeEvent(QCloseEvent *event); // Закрытие окна

signals:
    void close_signal(QString user, QString rights);
private:
    Ui::Entry *ui;
};

#endif // ENTRY_H
