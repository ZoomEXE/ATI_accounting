#ifndef ADDITEM_H
#define ADDITEM_H

#include <QDialog>
#include <product.h>

namespace Ui {
class AddItem;
}

class AddItem : public QDialog
{
    Q_OBJECT

public:
    explicit AddItem(int row, product item, QWidget *parent = nullptr);
    ~AddItem();
    int rowItem;
    QString meas; //Единица измерения

private slots:

    void on_pushButtonCancel_clicked();

    void on_pushButtonAdd_clicked();

signals:
    void dataToMainWindow(int row, product item);

private:
    Ui::AddItem *ui;
};

#endif // ADDITEM_H
