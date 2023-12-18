#ifndef ORDERDETAILS_H
#define ORDERDETAILS_H

#include <QDialog>
#include <QComboBox>
#include "order.h"

namespace Ui {
class OrderDetails;
}

class OrderDetails : public QDialog
{
    Q_OBJECT

public:
    explicit OrderDetails(order tempOrd, QWidget *parent = nullptr);
    ~OrderDetails();

private slots:
    void on_pushButtonSave_clicked();

private:
    Ui::OrderDetails *ui;
};

#endif // ORDERDETAILS_H
