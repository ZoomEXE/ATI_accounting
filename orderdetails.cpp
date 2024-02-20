#include "orderdetails.h"
#include "ui_orderdetails.h"

OrderDetails::OrderDetails(order tempOrd, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OrderDetails)
{
    ui->setupUi(this);
    ui->labelOrderName->setText(tempOrd.name);
    ui->tableWidgetOrderTop->setRowCount(1);
    ui->tableWidgetOrderTop->setItem(0, 0, new QTableWidgetItem(tempOrd.target));
    ui->tableWidgetOrderTop->setItem(0, 1, new QTableWidgetItem(tempOrd.base));
    ui->tableWidgetOrderTop->setItem(0, 2, new QTableWidgetItem(tempOrd.senders[0]));
    ui->tableWidgetOrderTop->setItem(0, 3, new QTableWidgetItem(tempOrd.recipients[0]));
    //ui->tableWidgetOrderTop->setItem(0, 4, new QTableWidgetItem(tempOrd.dateOutput));
    //ui->tableWidgetOrderTop->setItem(0, 5, new QTableWidgetItem(tempOrd.expirationDate));

    ui->label_8->setText("     " + tempOrd.dateOutput + " г.     ");
    ui->label_10->setText("     " + tempOrd.expirationDate + " г.     ");
    ui->tableWidgetOrderTop->item(0, 0)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetOrderTop->item(0, 1)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetOrderTop->item(0, 2)->setTextAlignment(Qt::AlignCenter);
    ui->tableWidgetOrderTop->item(0, 3)->setTextAlignment(Qt::AlignCenter);
    //ui->tableWidgetOrderTop->item(0, 4)->setTextAlignment(Qt::AlignCenter);
    //ui->tableWidgetOrderTop->item(0, 5)->setTextAlignment(Qt::AlignCenter);

    ui->tableWidgetOrdersProd->setRowCount(tempOrd.items.size());
    for(int i = 0; i < tempOrd.items.size(); ++i) {
        ui->tableWidgetOrdersProd->setItem(i, 0, new QTableWidgetItem(tempOrd.items[i].name));
        ui->tableWidgetOrdersProd->setItem(i, 1, new QTableWidgetItem(tempOrd.items[i].nomNumber));
        ui->tableWidgetOrdersProd->setItem(i, 2, new QTableWidgetItem(tempOrd.items[i].factoryNumber));
        ui->tableWidgetOrdersProd->setItem(i, 3, new QTableWidgetItem(tempOrd.items[i].releaseDate));
        ui->tableWidgetOrdersProd->setItem(i, 4, new QTableWidgetItem(QString::number(tempOrd.items[i].category)));
        ui->tableWidgetOrdersProd->setItem(i, 5, new QTableWidgetItem(tempOrd.items[i].measure));
        ui->tableWidgetOrdersProd->setItem(i, 6, new QTableWidgetItem(QString::number(tempOrd.items[i].count)));

        ui->tableWidgetOrdersProd->item(i, 0)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrdersProd->item(i, 1)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrdersProd->item(i, 2)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrdersProd->item(i, 3)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrdersProd->item(i, 4)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrdersProd->item(i, 5)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrdersProd->item(i, 6)->setTextAlignment(Qt::AlignCenter);

        double cnt = 0.0;
        /*QComboBox *combo = new QComboBox();
        for(int j = 0; j < tempOrd.orderDocs.size(); ++j) {
            for(int k = 0; k < tempOrd.orderDocs[j].items.size(); ++k) {
                if(tempOrd.items[i].name == tempOrd.orderDocs[j].items[k].name) {
                    cnt += tempOrd.orderDocs[j].items[k].count;
                    combo->addItem(tempOrd.orderDocs[j].name + " № " + tempOrd.orderDocs[j].numberOutput + " от " + tempOrd.orderDocs[j].dateOutput.toString("dd.MM.yyyy") + " | " + QString::number(tempOrd.orderDocs[j].items[k].count, 'g', 10) + " " + tempOrd.orderDocs[j].items[k].measure);
                }
            }
        }*/
        ui->tableWidgetOrdersProd->setItem(i, 7, new QTableWidgetItem(QString::number(cnt)));
        ui->tableWidgetOrdersProd->setItem(i, 11, new QTableWidgetItem(""));

        ui->tableWidgetOrdersProd->item(i, 7)->setTextAlignment(Qt::AlignCenter);
    }

    ui->sendType->setText(tempOrd.sendType);
    ui->tableWidgetOrdersProd->resizeColumnsToContents();
    ui->tableWidgetOrdersProd->resizeRowsToContents();
    ui->tableWidgetOrderTop->resizeColumnsToContents();
    ui->tableWidgetOrderTop->resizeRowsToContents();
    ord = tempOrd;
}

OrderDetails::~OrderDetails()
{
    delete ui;
}

void OrderDetails::on_pushButtonSave_clicked()
{
    OrderDetails::close();
}

void OrderDetails::on_pushButton_clicked()
{
    if(ui->pushButton->text() == "Редактировать") {
        ui->pushButton->setText("Сохранить");
        ui->tableWidgetOrdersProd->setEditTriggers(QAbstractItemView::DoubleClicked);
        ui->tableWidgetOrderTop->setEditTriggers(QAbstractItemView::DoubleClicked);
        //ui->tableWidgetOrdersProd->item(1, 11)->

    }else{

        ui->pushButton->setText("Редактировать");
        ui->tableWidgetOrdersProd->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->tableWidgetOrderTop->setEditTriggers(QAbstractItemView::NoEditTriggers);
        for(int i = 0; i < ui->tableWidgetOrdersProd->rowCount(); ++i) {
            ord.items[i].price = ui->tableWidgetOrdersProd->item(i, 7)->text().toDouble();
        }


        //Сохранение изменений наряда в БД
        order sendOrd;

        sendOrd.name = ui->labelOrderName->text();
        sendOrd.target = ui->tableWidgetOrderTop->item(0, 0)->text();
        sendOrd.base = ui->tableWidgetOrderTop->item(0, 1)->text();
        sendOrd.senders.push_back(ui->tableWidgetOrderTop->item(0, 2)->text());
        sendOrd.recipients.push_back(ui->tableWidgetOrderTop->item(0, 3)->text());

        sendOrd.dateOutput = ui->label_8->text().mid(5,10);
        sendOrd.expirationDate = ui->label_10->text().mid(5, 10);

        //ui->tableWidgetOrdersProd->setRowCount(tempOrd.items.size());
        for(int i = 0; i < ui->tableWidgetOrdersProd->rowCount(); ++i) {
            product item;
            doc tempDoc;
            item.name = ui->tableWidgetOrdersProd->item(i, 0)->text();
            item.nomNumber = ui->tableWidgetOrdersProd->item(i, 1)->text();
            item.factoryNumber = ui->tableWidgetOrdersProd->item(i, 2)->text();
            item.releaseDate = ui->tableWidgetOrdersProd->item(i, 3)->text();
            item.category = ui->tableWidgetOrdersProd->item(i, 4)->text().toInt();
            item.measure = ui->tableWidgetOrdersProd->item(i, 5)->text();
            item.count = ui->tableWidgetOrdersProd->item(i, 6)->text().toDouble();
            item.price = ui->tableWidgetOrdersProd->item(i, 7)->text().toDouble();
            tempDoc.name = ui->tableWidgetOrdersProd->item(i, 11)->text();
            sendOrd.items.push_back(item);
            sendOrd.orderDocs.push_back(tempDoc);
        }
        emit sendOrderChanges(sendOrd);
    }
}
