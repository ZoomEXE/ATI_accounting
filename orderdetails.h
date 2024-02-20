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
    order ord;
    ~OrderDetails();

private slots:
    void on_pushButtonSave_clicked();

    void on_pushButton_clicked();
signals:
    void sendOrderChanges (order sendOrd);

private:
    Ui::OrderDetails *ui;
};

#endif // ORDERDETAILS_H
