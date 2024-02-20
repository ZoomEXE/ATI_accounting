#include "prodinfo.h"
#include "ui_prodinfo.h"

ProdInfo::ProdInfo(QVector <product> tempProds, QVector <doc> tempDocs, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProdInfo)
{
    ui->setupUi(this);
    ui->tableWidget->resizeColumnsToContents();
    ui->nameMC->setText(tempProds[0].name);
    //ui->mol->addItem(tempProds[0].type117);
    ui->edIzm->setText(tempProds[0].measure);
    ui->nomNumb->setText(tempProds[0].nomNumber);

    QMap <QString, QVector <double> > podrSumms;

    QVector <double> tempZero;
    for(int i = 0; i < 6; ++i) {
        tempZero.push_back(0.0);
    }

    double summ[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    for (int i = 0; i < tempProds.size(); ++i) {
        summ[0] += tempProds[i].count;
        if(ui->mol->findText(tempProds[i].type117) == -1)
        ui->mol->addItem(tempProds[i].type117);
        summ[tempProds[i].category] += tempProds[i].count;
        podrSumms.insert(tempProds[i].type117, tempZero);
    }

    for (int i = 0; i < tempProds.size(); ++i) {
        podrSumms[tempProds[i].type117][tempProds[i].category] += tempProds[i].count;
        podrSumms[tempProds[i].type117][0] += tempProds[i].count;
    }

    int k = 19;
    for (auto i = podrSumms.cbegin(); i != podrSumms.cend(); ++i) {
        if(i.key() != "Лицевой счет № 1 (Авиационно-технический склад)") {
            ui->tableWidget->setColumnCount(ui->tableWidget->columnCount() + 6);

            ui->tableWidget->setHorizontalHeaderItem(k + 0, new QTableWidgetItem("Состоит всего - " + i.key()));
            ui->tableWidget->setHorizontalHeaderItem(k + 1, new QTableWidgetItem("I"));
            ui->tableWidget->setHorizontalHeaderItem(k + 2, new QTableWidgetItem("II"));
            ui->tableWidget->setHorizontalHeaderItem(k + 3, new QTableWidgetItem("III"));
            ui->tableWidget->setHorizontalHeaderItem(k + 4, new QTableWidgetItem("IV"));
            ui->tableWidget->setHorizontalHeaderItem(k + 5, new QTableWidgetItem("V"));

            ui->tableWidget->setItem(0, k + 0, new QTableWidgetItem(QString::number(i.value()[0])));
            ui->tableWidget->setItem(0, k + 1, new QTableWidgetItem(QString::number(i.value()[1])));
            ui->tableWidget->setItem(0, k + 2, new QTableWidgetItem(QString::number(i.value()[2])));
            ui->tableWidget->setItem(0, k + 3, new QTableWidgetItem(QString::number(i.value()[3])));
            ui->tableWidget->setItem(0, k + 4, new QTableWidgetItem(QString::number(i.value()[4])));
            ui->tableWidget->setItem(0, k + 5, new QTableWidgetItem(QString::number(i.value()[5])));

            k += 6;
        }
    }

    double summDocs[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    for (int i = 0; i < tempDocs.size(); ++i) {
        summDocs[0] += tempDocs[i].items[0].count;

        summDocs[tempDocs[i].items[0].category] += tempDocs[i].items[0].count;
        if(tempDocs[i].recipientOrg == "Лицевой счет № 1 (Авиационно-технический склад)") {
            podrSumms["Лицевой счет № 1 (Авиационно-технический склад)"][tempDocs[i].items[0].category] -= tempDocs[i].items[0].count;
            podrSumms["Лицевой счет № 1 (Авиационно-технический склад)"][0] -= tempDocs[i].items[0].count;
        }else{
            podrSumms[tempDocs[i].items[0].name][tempDocs[i].items[0].category] -= tempDocs[i].items[0].count;
            podrSumms[tempDocs[i].items[0].name][0] -= tempDocs[i].items[0].count;
        }

    }

    double startSumm[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    for(int i = 0; i < 6; ++i) {
        startSumm[i] = summ[i] - summDocs[i];
    }

    ui->tableWidget->setItem(0, 0, new QTableWidgetItem("01.01.2023")); //Дата импорта данных в БД (внести сохранение даты при импорте)
    ui->tableWidget->setItem(0, 1, new QTableWidgetItem("Импорт в БД"));
    ui->tableWidget->setItem(0, 2, new QTableWidgetItem("-"));
    ui->tableWidget->setItem(0, 3, new QTableWidgetItem("01.01.2023"));
    ui->tableWidget->setItem(0, 4, new QTableWidgetItem("-"));
    ui->tableWidget->setItem(0, 5, new QTableWidgetItem(QString::number(startSumm[0])));
    ui->tableWidget->setItem(0, 6, new QTableWidgetItem("-"));
    ui->tableWidget->setItem(0, 7, new QTableWidgetItem(QString::number(startSumm[0])));
    ui->tableWidget->setItem(0, 8, new QTableWidgetItem(QString::number(startSumm[1])));
    ui->tableWidget->setItem(0, 9, new QTableWidgetItem(QString::number(startSumm[2])));
    ui->tableWidget->setItem(0, 10, new QTableWidgetItem(QString::number(startSumm[3])));
    ui->tableWidget->setItem(0, 11, new QTableWidgetItem(QString::number(startSumm[4])));
    ui->tableWidget->setItem(0, 12, new QTableWidgetItem(QString::number(startSumm[5])));

    ui->tableWidget->setItem(0, 13, new QTableWidgetItem("0"));
    ui->tableWidget->setItem(0, 14, new QTableWidgetItem("0"));
    ui->tableWidget->setItem(0, 15, new QTableWidgetItem("0"));
    ui->tableWidget->setItem(0, 16, new QTableWidgetItem("0"));
    ui->tableWidget->setItem(0, 17, new QTableWidgetItem("0"));
    ui->tableWidget->setItem(0, 18, new QTableWidgetItem("0"));

    for(auto it = podrSumms.cbegin(); it != podrSumms.cend(); ++it) {
        if(it.key() == "Лицевой счет № 1 (Авиационно-технический склад)") {
            ui->tableWidget->setItem(0, 13, new QTableWidgetItem(QString::number(podrSumms["Лицевой счет № 1 (Авиационно-технический склад)"][0])));
            ui->tableWidget->setItem(0, 14, new QTableWidgetItem(QString::number(podrSumms["Лицевой счет № 1 (Авиационно-технический склад)"][1])));
            ui->tableWidget->setItem(0, 15, new QTableWidgetItem(QString::number(podrSumms["Лицевой счет № 1 (Авиационно-технический склад)"][2])));
            ui->tableWidget->setItem(0, 16, new QTableWidgetItem(QString::number(podrSumms["Лицевой счет № 1 (Авиационно-технический склад)"][3])));
            ui->tableWidget->setItem(0, 17, new QTableWidgetItem(QString::number(podrSumms["Лицевой счет № 1 (Авиационно-технический склад)"][4])));
            ui->tableWidget->setItem(0, 18, new QTableWidgetItem(QString::number(podrSumms["Лицевой счет № 1 (Авиационно-технический склад)"][5])));
        }
    }

    for(int i = 0; i < tempDocs.size(); ++i) {
        ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
        for (int q = 13; q < ui->tableWidget->columnCount() - 1; ++q) {
            ui->tableWidget->setItem(i + 1, q, new QTableWidgetItem("0"));
        }

        ui->tableWidget->setItem(i + 1, 0, new QTableWidgetItem(tempDocs[i].dateInput.toString("dd.MM.yyyy"))); //Дата записи
        ui->tableWidget->setItem(i + 1, 1, new QTableWidgetItem(tempDocs[i].name));
        ui->tableWidget->setItem(i + 1, 2, new QTableWidgetItem(tempDocs[i].numberOutput));
        ui->tableWidget->setItem(i + 1, 3, new QTableWidgetItem(tempDocs[i].dateOutput.toString("dd.MM.yyyy")));
        ui->tableWidget->setItem(i + 1, 4, new QTableWidgetItem(tempDocs[i].senderOrg));
        ui->tableWidget->setItem(i + 1, 5, new QTableWidgetItem(QString::number(summDocs[0],'g', 2)));
        ui->tableWidget->setItem(i + 1, 6, new QTableWidgetItem("-"));
        ui->tableWidget->setItem(i + 1, 7, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, 7)->text().toDouble() + summDocs[0])));
        ui->tableWidget->setItem(i + 1, 8, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, 8)->text().toDouble() + summDocs[1])));
        ui->tableWidget->setItem(i + 1, 9, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, 9)->text().toDouble() + summDocs[2])));
        ui->tableWidget->setItem(i + 1, 10, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, 10)->text().toDouble() + summDocs[3])));
        ui->tableWidget->setItem(i + 1, 11, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, 11)->text().toDouble() + summDocs[4])));
        ui->tableWidget->setItem(i + 1, 12, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, 12)->text().toDouble() + summDocs[5])));
        if(tempDocs[i].recipientOrg == "Лицевой счет № 1 (Авиационно-технический склад)") {
            ui->tableWidget->setItem(i + 1, 13, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, 13)->text().toDouble() + summDocs[0])));
            ui->tableWidget->setItem(i + 1, 14, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, 14)->text().toDouble() + summDocs[1])));
            ui->tableWidget->setItem(i + 1, 15, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, 15)->text().toDouble() + summDocs[2])));
            ui->tableWidget->setItem(i + 1, 16, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, 16)->text().toDouble() + summDocs[3])));
            ui->tableWidget->setItem(i + 1, 17, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, 17)->text().toDouble() + summDocs[4])));
            ui->tableWidget->setItem(i + 1, 18, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, 18)->text().toDouble() + summDocs[5])));
        }else{
            int l = findColumn(tempDocs[i].recipientOrg);
            ui->tableWidget->setItem(i + 1, l, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, l)->text().toDouble() + summDocs[0])));
            ui->tableWidget->setItem(i + 1, l + 1, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, l + 1)->text().toDouble() + summDocs[1])));
            ui->tableWidget->setItem(i + 1, l + 2, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, l + 2)->text().toDouble() + summDocs[2])));
            ui->tableWidget->setItem(i + 1, l + 3, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, l +3)->text().toDouble() + summDocs[3])));
            ui->tableWidget->setItem(i + 1, l + 4, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, l + 4)->text().toDouble() + summDocs[4])));
            ui->tableWidget->setItem(i + 1, l + 5, new QTableWidgetItem(QString::number(ui->tableWidget->item(i, l + 5)->text().toDouble() + summDocs[5])));
        }
    }

    ui->tableWidget->setHorizontalHeaderItem(ui->tableWidget->columnCount() - 1, new QTableWidgetItem(""));
    ui->tableWidget->resizeColumnsToContents();

    for(int i = 13; i < ui->tableWidget->columnCount() - 1; ++i) {
        for (int j = 1; j < ui->tableWidget->rowCount(); ++j) {
            if(ui->tableWidget->item(j, i)->text() == "0") {
                ui->tableWidget->setItem(j, i, new QTableWidgetItem(ui->tableWidget->item(j - 1, i)->text()));
            }
        }
    }

}
int ProdInfo::findColumn(QString mol) {
    for(int  i = 19; i < ui->tableWidget->columnCount(); ++i) {
        if(ui->tableWidget->horizontalHeaderItem(i)->text() == "Состоит всего - " + mol) {
            return i;
        }
    }
    return 7;
}

ProdInfo::~ProdInfo()
{
    delete ui;
}

void ProdInfo::on_mol_activated(int index)
{

}

void ProdInfo::on_mol_activated(const QString &arg1)
{
    if(arg1 == "Лицевой счет № 1 (Авиационно-технический склад)") {
        ui->tableWidget->selectColumn(13);
    }else{
        ui->tableWidget->selectColumn(findColumn(arg1));
    }

}
