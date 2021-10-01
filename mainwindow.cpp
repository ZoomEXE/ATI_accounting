#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <entry.h>
#include <QtSql>
//#include "header/xlsxdocument.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    Entry *ent = new Entry;
    QObject::connect(ent, SIGNAL(close_signal()), this, SLOT(close_entry()));
    ent->setModal(true);
    ent->exec();
    ui->setupUi(this);
    save_first_tabs();
    QSqlDatabase sdb = QSqlDatabase::addDatabase("QSQLITE");
    sdb.setDatabaseName("ats_db.db");
    if(!sdb.open()) {
        qDebug() << sdb.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    // Создание Checkable ComboBox с категориями
    this->model = new QStandardItemModel;
    this->categ_1 = new QStandardItem;
    this->categ_2 = new QStandardItem;
    this->categ_3 = new QStandardItem;
    this->categ_4 = new QStandardItem;
    this->categ_5 = new QStandardItem;

    // Добавление категорий в модель
    this->categ_1->setText("I категория");
    this->categ_1->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    this->categ_1->setData(Qt::Unchecked, Qt::CheckStateRole);
    this->model->insertRow(0, this->categ_1);

    this->categ_2->setText("II категория");
    this->categ_2->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    this->categ_2->setData(Qt::Unchecked, Qt::CheckStateRole);
    this->model->insertRow(1, this->categ_2);

    this->categ_3->setText("III категория");
    this->categ_3->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    this->categ_3->setData(Qt::Unchecked, Qt::CheckStateRole);
    this->model->insertRow(2, this->categ_3);

    this->categ_4->setText("IV категория");
    this->categ_4->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    this->categ_4->setData(Qt::Unchecked, Qt::CheckStateRole);
    this->model->insertRow(3, this->categ_4);

    this->categ_5->setText("V категория");
    this->categ_5->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    this->categ_5->setData(Qt::Unchecked, Qt::CheckStateRole);
    this->model->insertRow(4, this->categ_5);

    ui->category_comboBox->setModel(this->model);
    connect(ui->category_comboBox, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(slotModelItemChanged(QStandardItem*)));

    ////////////////////////////////////////////////////////////////////////////////
}

void MainWindow::slotModelItemChanged(QStandardItem *item)
{
    if (item->data(Qt::CheckStateRole).toInt() == Qt::Checked) {
        qDebug() << "Succes";
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

//Сохранение первых вкладок
void MainWindow::save_first_tabs()
{
    while (ui->tabWidget->count() > 0)
    {
        on_tabWidget_tabCloseRequested(0);
    }
}

void MainWindow::on_log_out_triggered()
{
    Entry *ent = new Entry;
    QObject::connect(ent, SIGNAL(close_signal()), this, SLOT(close_entry()));
    ent->setModal(true);
    ent->exec();
}
//Закрытие окна входа
void MainWindow::close_entry() {
    MainWindow::close();
}
//Удаление вкладки
void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    hidden_tabs->insert(ui->tabWidget->tabText(index), ui->tabWidget->widget(index));
    ui->tabWidget->removeTab(index);
}

//Открытие новой вкладки
void MainWindow::new_tab(QString name)
{
    auto tab = hidden_tabs->take(name);
    ui->tabWidget->addTab(tab, name);
    ui->tabWidget->setCurrentIndex(ui->tabWidget->count() - 1);
}

//Проверка на наличие открытой вкладки
bool MainWindow::check_available_tab(QString name)
{
    bool tab_is_absent = true;
    for (int i = 0; i < ui->tabWidget->count(); ++i)
    {
        if(ui->tabWidget->tabText(i) == name)
        {
            tab_is_absent = false;
            ui->tabWidget->setCurrentIndex(i);
            return tab_is_absent;
        }
    }
    return tab_is_absent;
}

//Открытие вкладок////////////////////////////////////////////////
void MainWindow::on_spr_ati_triggered()                         //
{                                                               //

    if (check_available_tab("Справочник АТИ"))
    {
        new_tab("Справочник АТИ");
    }
}

void MainWindow::on_spr_pptn_2_triggered()
{
    if (check_available_tab("Справочник ППТН"))
    {
        new_tab("Справочник ППТН");
    }
}


void MainWindow::on_info_triggered()
{
    if (check_available_tab("О программе"))
    {
        new_tab("О программе");
    }
}

void MainWindow::on_edit_profile_data_triggered()
{
    if (check_available_tab("Редактировать данные"))
    {
        new_tab("Редактировать данные");
    }
}



void MainWindow::on_change_pass_triggered()
{
    if (check_available_tab("Изменить пароль"))
    {
        new_tab("Изменить пароль");
    }
}

void MainWindow::on_action_users_edit_triggered()
{
    if (check_available_tab("Пользователи"))
    {
        new_tab("Пользователи");
    }
}

void MainWindow::on_action_podr_edit_triggered()
{
    if (check_available_tab("Подразделения"))
    {
        new_tab("Подразделения");
    }
}

void MainWindow::on_action_sklad_triggered()
{
    if (check_available_tab("Учет в в/ч"))
    {
        new_tab("Учет в в/ч");
    }
}

void MainWindow::on_action_remont_triggered()
{
    if (check_available_tab("Отправлены на ремонт"))
    {
        new_tab("Отправлены в ремонт");
    }
}

void MainWindow::on_action_prihod_triggered()
{
    if (check_available_tab("Приход"))
    {
        new_tab("Расход");
    }
}

void MainWindow::on_action_rashod_triggered()
{
    if (check_available_tab("Расход"))
    {
        new_tab("Расход");
    }                                                           //
}                                                               //
                                                                //
/////////////////////////////////////////////////////////////////

//Изменение критериев поиска
void MainWindow::on_comboBoxFindtypeAti_activated(int index)
{
    if(index == 0) ui->labelSprAti->setText("Введите номенклатурный номер:");
    if(index == 1) ui->labelSprAti->setText("Введите наименование:");
}

void MainWindow::on_comboBoxfindTypePptn_activated(int index)
{
    if(index == 0) ui->labelSprPptn->setText("Введите номенклатурный номер:");
    if(index == 1) ui->labelSprPptn->setText("Введите наименование:");
}

//Поиск в справочнике АТИ по н/н и наименованию
void MainWindow::on_findInAtiButton_clicked()
{
    QSqlQuery query;
    QString text;
    QSqlQueryModel *modelFind = new QSqlQueryModel;
    // Поиск по н/н
    if(ui->comboBoxFindtypeAti->currentIndex() == 0) {
        text = "SELECT * FROM spr_ati WHERE nomkl_nom like " + ui->lineEdit_sprAti->text();
        query.exec(text);
        qDebug() << query.lastError();
        while(query.next())
            qDebug() << query.value(1).toString();
    }else{
        //Поиск по наименованию
        text = "SELECT * FROM spr_ati WHERE name like \'%" + ui->lineEdit_sprAti->text().toUpper() + "%\'";
        query.exec(text);
        qDebug() << query.lastError();
        while(query.next())
            qDebug() << query.value(1).toString();
    }
    //Отображение результатов
    modelFind->setQuery(query);
    modelFind->setHeaderData(0,Qt::Horizontal, "Номенкл. номер");
    modelFind->setHeaderData(1,Qt::Horizontal, "Наименование");
    modelFind->setHeaderData(2,Qt::Horizontal, "Ед. измерения");
    modelFind->setHeaderData(3,Qt::Horizontal, "Примечание");
    ui->spr_ati_tableView->setModel(modelFind);
    QFont regularFont ("MS Shell Dig 2", 9,QFont::Bold);
    ui->spr_ati_tableView->horizontalHeader()->setFont(regularFont);
    ui->spr_ati_tableView->resizeColumnsToContents();
    ui->spr_ati_tableView->show();
}

// Отображение выбранной группы АТИ
void MainWindow::on_findGroup_inAtiButton_clicked()
{
    QString text = "SELECT * FROM spr_ati WHERE nomkl_nom like \'17" + ui->comboBoxGroup_spr_ati->currentText().left(3) + "_____\'";
    QSqlQuery query;
    query.exec(text);
    qDebug() << query.lastError();
    while(query.next())
        qDebug() << query.value(1).toString();
    QSqlQueryModel *modelGroup = new QSqlQueryModel;
    modelGroup->setQuery(query);
    modelGroup->setHeaderData(0,Qt::Horizontal, "Номенкл. номер");
    modelGroup->setHeaderData(1,Qt::Horizontal, "Наименование");
    modelGroup->setHeaderData(2,Qt::Horizontal, "Ед. измерения");
    modelGroup->setHeaderData(3,Qt::Horizontal, "Примечание");
    ui->spr_ati_tableView->setModel(modelGroup);
    QFont regularFont ("MS Shell Dig 2", 9,QFont::Bold);
    ui->spr_ati_tableView->horizontalHeader()->setFont(regularFont);
    ui->spr_ati_tableView->resizeColumnsToContents();
    ui->spr_ati_tableView->show();
}

void MainWindow::on_findInPptnButton_clicked()
{
    QSqlQuery query;
    QString text;
    QSqlQueryModel *modelFind = new QSqlQueryModel;
    // Поиск по н/н
    if(ui->comboBoxfindTypePptn->currentIndex() == 0) {
        text = "SELECT * FROM spr_pptn WHERE nomkl_nom like " + ui->lineEdit_sprPptn->text();
        query.exec(text);
        qDebug() << query.lastError();
        while(query.next())
            qDebug() << query.value(1).toString();
    }else{
        //Поиск по наименованию
        text = "SELECT * FROM spr_pptn WHERE name like \'%" + ui->lineEdit_sprPptn->text().toUpper() + "%\'";
        query.exec(text);
        qDebug() << query.lastError();
        while(query.next())
            qDebug() << query.value(1).toString();
    }
    //Отображение результатов
    modelFind->setQuery(query);
    modelFind->setHeaderData(0,Qt::Horizontal, "Номенкл. номер");
    modelFind->setHeaderData(1,Qt::Horizontal, "Наименование");
    modelFind->setHeaderData(2,Qt::Horizontal, "Ед. измерения");
    modelFind->setHeaderData(3,Qt::Horizontal, "Примечание");
    ui->spr_pptn_tableView->setModel(modelFind);
    QFont regularFont ("MS Shell Dig 2", 9,QFont::Bold);
    ui->spr_ati_tableView->horizontalHeader()->setFont(regularFont);
    ui->spr_pptn_tableView->resizeColumnsToContents();
    ui->spr_pptn_tableView->show();
}
