#include "additembn.h"
#include "ui_additembn.h"

addItemBN::addItemBN(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addItemBN)
{
    ui->setupUi(this);
    QSqlDatabase sdb = QSqlDatabase::addDatabase("QSQLITE");
    sdb.setDatabaseName("ats_db.db");

    if(!sdb.open()) {
        qDebug() << sdb.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }
    baseDB = sdb;

    QSqlQuery query(baseDB);
    query.exec("SELECT * FROM spr_ati");
    QString text;
    QSqlQueryModel *modelFind = new QSqlQueryModel;
        qDebug() << query.lastError();

    //Отображение результатов
    modelFind->setQuery(query);
    modelFind->setHeaderData(0,Qt::Horizontal, "Номенкл. номер");
    modelFind->setHeaderData(1,Qt::Horizontal, "Наименование");
    modelFind->setHeaderData(2,Qt::Horizontal, "Ед. измерения");
    modelFind->setHeaderData(3,Qt::Horizontal, "Примечание");
    ui->tableViewNomklVDO->setModel(modelFind);
    QFont regularFont ("MS Shell Dig 2", 9,QFont::Bold);
    ui->tableViewNomklVDO->horizontalHeader()->setFont(regularFont);
    ui->tableViewNomklVDO->resizeColumnsToContents();
    ui->tableViewNomklVDO->show();
}

addItemBN::~addItemBN()
{
    delete ui;
}

//Поиск в номенклаторе
void addItemBN::on_lineEditFind_textChanged(const QString &arg1)
{
    QSqlQuery query (baseDB);
    QString text;
    QSqlQueryModel *modelFind = new QSqlQueryModel;

        text = "SELECT * FROM spr_ati WHERE nomkl_nom like \'%" + arg1.toUpper() + "%\'" + " OR name like \'%" + arg1.toUpper() + "%\' ";
        query.exec(text);
        qDebug() << query.lastError();
        //while(query.next())
         //   qDebug() << query.value(1).toString();

    //Отображение результатов
    modelFind->setQuery(query);
    modelFind->setHeaderData(0,Qt::Horizontal, "Номенкл. номер");
    modelFind->setHeaderData(1,Qt::Horizontal, "Наименование");
    modelFind->setHeaderData(2,Qt::Horizontal, "Ед. измерения");
    modelFind->setHeaderData(3,Qt::Horizontal, "Примечание");
    ui->tableViewNomklVDO->setModel(modelFind);
    QFont regularFont ("MS Shell Dig 2", 9,QFont::Bold);
    ui->tableViewNomklVDO->horizontalHeader()->setFont(regularFont);
    ui->tableViewNomklVDO->resizeColumnsToContents();
    ui->tableViewNomklVDO->show();
}

//Двойной щелчок по номенклатору/заполнение полей из номенклатора
void addItemBN::on_tableViewNomklVDO_doubleClicked(const QModelIndex &index)
{
    ui->lineEditNomNumb_2->setText(index.sibling(index.row(), 0).data().toString());
    ui->lineEditName_2->setText(index.sibling(index.row(), 1).data().toString());
    ui->lineEditMeasure_2->setText(index.sibling(index.row(), 2).data().toString());
}

//Нажатие кнопик "Отмена"
void addItemBN::on_pushButtonCancel_2_clicked()
{
    addItemBN::close();
}

//Нажатие кнопки "Добавить"
void addItemBN::on_pushButtonAdd_2_clicked()
{
    product temp;
    temp.name = ui->lineEditName_2->text();
    temp.nomNumber = ui->lineEditNomNumb_2->text();
    temp.category = ui->lineEditCategory_2->text().toInt();
    temp.measure = ui->lineEditMeasure_2->text();
    temp.count = ui->SpinBoxCount_2->value();
    temp.factoryNumber = ui->serialNumb_2->text();
    temp.releaseDate = ui->date_2->text();
    emit dataToMainWindow(-2, temp);
    addItemBN::close();

}
