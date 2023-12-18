#include "additem.h"
#include "ui_additem.h"

AddItem::AddItem(int row, product item, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddItem)
{
    ui->setupUi(this);

    ui->lineEditName->setText(item.name);
    if(ui->lineEditName->text() == "") ui->lineEditName->setReadOnly(false);
    ui->lineEditNomNumb->setText(item.nomNumber);
    if(ui->lineEditNomNumb->text() == "") ui->lineEditNomNumb->setReadOnly(false);
    ui->lineEditCategory->setText(QString::number(item.category));
    if(row == -1) ui->lineEditCategory->setReadOnly(false);
    rowItem = row;
    //if(row !=-1)
    //ui->SpinBoxCount->setMaximum(item.count);
    meas = item.measure;
    ui->lineEditMeasure->setText(meas);
    if(ui->lineEditMeasure->text() == "") ui->lineEditMeasure->setReadOnly(false);
}

AddItem::~AddItem()
{
    delete ui;
}

//Нажатие кнопки "Отмена"
void AddItem::on_pushButtonCancel_clicked()
{
    AddItem::close();
}

//Нажатие кнопки "Добавить"
void AddItem::on_pushButtonAdd_clicked()
{
    product temp;
    temp.name = ui->lineEditName->text();
    temp.nomNumber = ui->lineEditNomNumb->text();
    temp.category = ui->lineEditCategory->text().toInt();
    temp.measure = meas;
    temp.count = ui->SpinBoxCount->value();
    temp.factoryNumber = ui->serialNumb->text();
    temp.releaseDate = ui->date->text();
    emit dataToMainWindow(rowItem, temp);
    AddItem::close();
}
