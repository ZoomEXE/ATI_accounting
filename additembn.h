#ifndef ADDITEMBN_H
#define ADDITEMBN_H

#include <QWidget>
#include <QtSql>
#include <QDialog>
#include <product.h>

namespace Ui {
class addItemBN;
}

class addItemBN : public QDialog
{
    Q_OBJECT

public:
    explicit addItemBN(QWidget *parent = nullptr);
    ~addItemBN();
    QSqlDatabase baseDB; //Переменная для хранения связи с БД

private slots:
    void on_lineEditFind_textChanged(const QString &arg1);

    void on_tableViewNomklVDO_doubleClicked(const QModelIndex &index);

    void on_pushButtonCancel_2_clicked();

    void on_pushButtonAdd_2_clicked();

signals:
    void dataToMainWindow(int row, product item);

private:
    Ui::addItemBN *ui;
};

#endif // ADDITEMBN_H
