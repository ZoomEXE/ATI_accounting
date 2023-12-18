#include "entry.h"
#include "ui_entry.h"

Entry::Entry(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Entry)
{
    ui->setupUi(this);
    QSqlDatabase sdb = QSqlDatabase::addDatabase("QSQLITE"); //Подключение к базе данных
    sdb.setDatabaseName("ats_db.db");
    if(!sdb.open()) {
        qDebug() << sdb.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    QSqlQuery query(sdb);
    query.exec("SELECT login FROM users");
    //query.next();
    while(query.next()) {
        ui->select_user->addItem(query.value(0).toString()); //Заполнение Комбобокса Логинами из базы данных
    }
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
    QSqlDatabase sdb = QSqlDatabase::addDatabase("QSQLITE");
    sdb.setDatabaseName("ats_db.db");
    if(!sdb.open()) {
        qDebug() << sdb.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    QSqlQuery query(sdb);
    currUser = ui->select_user->currentText();
    QString queryText = "SELECT password FROM users WHERE login = '" + currUser + "'";
    qDebug() << queryText;
    query.exec(queryText);
    query.next();
    qDebug() << ui->passwd_edit->text() << " || " << query.value(0).toString();
    if(ui->passwd_edit->text() == query.value(0).toString()) {

        queryText = "SELECT rights FROM users WHERE login = '" + currUser + "'";
        query.exec(queryText);
        query.next();
        currUserRights = query.value(0).toString();
        emit close_signal(currUser, currUserRights);
        Entry::close();
        //emit close_signal(currUser, currUserRights);
    }else{
        QMessageBox msg;
        msg.setWindowTitle("Внимание");
        msg.setText("Введенные пароль/логин неверны!");
        msg.exec();
    }

}
void Entry::closeEvent(QCloseEvent *event) {
    emit close_signal(currUser, currUserRights);
    QWidget::closeEvent(event);
}
