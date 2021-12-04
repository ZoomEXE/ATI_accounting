#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace QXlsx;
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

    ui->pushButton_2->setText("Удалить текущий \n лицевой счет");
    ui->pushButton_3->setText("Добавить текущий \n лицевой счет");
    ui->pushButton_4->setText("Добавить все лицевые счета");

    //Заполнение заголовков в таблице импорта
    ui->tableWidget->setColumnCount(5);
    ui->tableAccount->setColumnCount(5);
    QStringList headers;
    headers.append("Наименование");
    headers.append("Ном. номер");
    headers.append("Ед. изм.");
    headers.append("Количество");
    headers.append("Цена");
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    ui->tableAccount->setHorizontalHeaderLabels(headers);
    ui->tableAccount->horizontalHeader()->setStretchLastSection(true);

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

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    importBD(); //Импортирование из БД

}

//Функция импорта из БД
void MainWindow::importBD()
{
    //Заполнение контейнера АТИ данными из БД
    QString queryText = "SELECT * FROM lic_scheta";
    QSqlQuery query, queryProduct;
    query.exec(queryText);
    qDebug() << query.lastError();
    while(query.next())
    {
        //Считывание лицевых счетов
        QString id = query.value(0).toString();
        QString schet = query.value(1).toString();
        QVector <product> currentSchet;

        queryText = "SELECT * FROM product_ati WHERE id_lic_scheta = " + id;
        queryProduct.exec(queryText);
        qDebug() << queryProduct.lastError();

        while(queryProduct.next())
        {
            //Считывание АТИ из лицевого счета
            product temp;
            temp.name = queryProduct.value(1).toString();
            temp.count = queryProduct.value(5).toDouble();
            temp.price = queryProduct.value(3).toDouble();
            temp.measure = queryProduct.value(4).toString();
            temp.category = queryProduct.value(7).toInt();
            temp.nomNumber = queryProduct.value(2).toString();
            temp.releaseDate = QDate::fromString(queryProduct.value(8).toString());
            temp.factoryNumber = queryProduct.value(6).toString();

            currentSchet.push_back(temp);
        }

        dataATI.insert(schet, currentSchet);
    }
    //Заполнение комбобокса лицевыми счетами из временного контейнера QMap
    QMapIterator <QString, QVector <product>> it(dataATI);
    while(it.hasNext()) {
        it.next();
        ui->comboBoxScheta->addItem(it.key());
    }
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
    if (check_available_tab("Приходные документы"))
    {
        new_tab("Приходные документы");
    }
}

void MainWindow::on_action_rashod_triggered()
{
    if (check_available_tab("Расходные документы"))
    {
        new_tab("Расходные документы");
    }
}

//Открыть вкладку "Распоряжения"
void MainWindow::on_orders_triggered()
{
    if (check_available_tab("Распоряжения"))
    {
        new_tab("Распоряжения");
    }
}

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

// Открытие вкладки импорта из Excel
void MainWindow::on_import_Excel_triggered()
{
    if (check_available_tab("Импорт из Excel"))
    {
        new_tab("Импорт из Excel");
    }
}

// Выбор файла Excel для импорта и заполнение временного контейнера QMap
void MainWindow::on_importExcelpushButton_clicked()
{
    QString fileExcel = QFileDialog::getOpenFileName(this, "Выберите файл для импорта...", "", "*.xlsx");
    ui->lineEdit->setText(fileExcel);
    if(fileExcel != "")
    {
        Document import(fileExcel);

        int i = 4;
        scheta.clear();
        while(import.read(i, 1).toString() != "Итого") {
            QString schet;
            int sum = 0;
           if(import.read(i, 2).toString() == "") {
               schet = import.read(i, 1).toString();
               sum = import.read(i, 5).toInt();
               i ++;
           }
           //Заполнение вектора имуществом
           QVector <product> prodsScheta;
           while(prodsScheta.size() < sum) {
               product temp;
               temp.name = import.read(i, 2).toString();
               temp.measure = import.read(i, 3).toString();
               temp.count = import.read(i, 5).toDouble();
               temp.price = import.read(i, 6).toDouble();
               prodsScheta.push_back(temp);
               i++;
           }
           scheta.insert(schet, prodsScheta);
        }
        //Заполнение комбобокса лицевыми счетами из временного контейнера QMap
        QMapIterator <QString, QVector <product>> it(scheta);
        while(it.hasNext()) {
            it.next();
            ui->comboBox_2->addItem(it.key());
        }
        ui->statusBar->showMessage("Файл Excel импортирован.");
    }else{
        QMessageBox msg;
        msg.setWindowTitle("Внимание!");
        msg.setText("Выберите файл для импорта!");
        msg.setIcon(QMessageBox::Critical);
        msg.exec();
    }
}

//Заполнение таблицы из временного контейнера
void MainWindow::on_comboBox_2_activated(const QString &arg1)
{
    QVector <product> currentSchet = scheta [arg1];
    int len = currentSchet.size();

    ui->tableWidget->setRowCount(len);

    for (int i = 0; i < len; ++i) {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(currentSchet[i].name));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(currentSchet[i].nomNumber));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(currentSchet[i].measure));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(currentSchet[i].count)));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(currentSchet[i].price,'g', 10)));
    }
    ui->tableWidget->resizeColumnsToContents();
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
}

//Добавление отдельного лицевого счета
void MainWindow::add_lic_schet(QString arg)
{
    if(ui->comboBox_2->count() != 0)
    {
        //Выбор типа имущества
        QString typeProduct;
        if(ui->comboBox_4->currentText() == "АТИ")
        {
            typeProduct = "product_ati";
        }else{
            typeProduct = "product_pptn";
        }

        QVector <product> currentSchet = scheta [arg];
        int len = currentSchet.size();
        QString queryText;
        QSqlQuery query;
        //Проверка наличия лицевого счета в БД
        queryText = "SELECT title FROM lic_scheta";
        query.exec(queryText);
        qDebug() << query.lastError();
        bool emptyFlag = false;
        while(query.next())
        {
            qDebug() << query.value(0);
            if(arg == query.value(0)){
                QMessageBox msg;
                msg.setWindowTitle("Внимание!");
                msg.setText("Данный лицевой счет уже существует в базе данных!");
                msg.exec();
                emptyFlag = true;
                ui->comboBox_2->removeItem(ui->comboBox_2->currentIndex());
                break;
            }
        }

        //Проверка на наличие лицевого счета
        if(!emptyFlag)
        {
            //Добавление лицевого счета
            queryText = "INSERT INTO lic_scheta (title) VALUES (\"" + arg + "\")";
            query.exec(queryText);
            qDebug() << query.lastError();

            //Вытаскиваем id лицевого счета
            queryText = "SELECT id FROM lic_scheta WHERE title = \"" + arg + "\"";
            query.exec(queryText);
            qDebug() << query.lastError();
            query.next();
            qDebug() << query.lastError();
            int id = query.value(0).toInt();

            //Добавление имущества в лицевой счет
            for (int i = 0; i < len; ++i) {
                queryText = "INSERT INTO " + typeProduct + " (name, price, measure, count, id_lic_scheta) VALUES (\"" + currentSchet[i].name + "\", \"" + QString::number(currentSchet[i].price, 'g', 10) + "\", \"" +
                                           currentSchet[i].measure + "\", \"" + QString::number(currentSchet[i].count, 'g', 10) + "\", \"" + QString::number(id) + "\")";
                qDebug() << queryText;
                query.exec(queryText);
                qDebug() << query.lastError();
            }
        }
        ui->tableWidget->setRowCount(0);
        importBD();
    }
}

//Нажатие кнопки "Добавить ЛС"
void MainWindow::on_pushButton_3_clicked()
{
    add_lic_schet(ui->comboBox_2->currentText());

    //Удаление лицевого списка из временного хранилища и комбобокса
    QString msgText = ui->comboBox_2->currentText();
    ui->statusBar->showMessage("Лицевой счет \"" + msgText + "\" добавлен в базу данных.");
    ui->comboBox_2->removeItem(ui->comboBox_2->currentIndex());
}

//Нажатие кнопки "Удалить ЛС"
void MainWindow::on_pushButton_2_clicked()
{
    QString msgText = ui->comboBox_2->currentText();
    ui->comboBox_2->removeItem(ui->comboBox_2->currentIndex());
    ui->tableWidget->setRowCount(0);
    ui->statusBar->showMessage("Лицевой счет \"" + msgText + "\" удален.");
}

//Нажатие кнопки "Добавить все ЛС"
void MainWindow::on_pushButton_4_clicked()
{
    ui->statusBar->showMessage("Ожидайте...");
    //int maxcount = ui->comboBox_2->count();
    while(ui->comboBox_2->count() > 0)
    {
        //Отображение прогресса выполнения
        /*int count = ui->comboBox_2->count();
        double progress = 100;
        if(maxcount != 0)
        {
            progress = (maxcount - count) / maxcount * 100;
        }
        ui->statusBar->showMessage("Прогресс выполнения - " + QString::number(progress, 'g', 2) + " %.", 100);
*/
        add_lic_schet(ui->comboBox_2->currentText());
        ui->comboBox_2->removeItem(ui->comboBox_2->currentIndex());
    }
    ui->statusBar->showMessage("Все лицевые счета из импортированного файла Excel добавлены в базу данных.");
}

//Отображение имущества на выбранном лицевом счете
void MainWindow::on_comboBoxScheta_activated(const QString &arg1)
{
    QVector <product> currentSchet;
    //Выбор типа имущества
    if(ui->comboBoxTypeProduct->currentText() == "АТИ")
    {
        currentSchet = dataATI[arg1];
    }else{
        currentSchet = dataPPTN[arg1];
    }

    int len = currentSchet.size();

    ui->tableAccount->setRowCount(len);

    for (int i = 0; i < len; ++i) {
        ui->tableAccount->setItem(i, 0, new QTableWidgetItem(currentSchet[i].name));
        ui->tableAccount->setItem(i, 1, new QTableWidgetItem(currentSchet[i].nomNumber));
        ui->tableAccount->setItem(i, 2, new QTableWidgetItem(currentSchet[i].measure));
        ui->tableAccount->setItem(i, 3, new QTableWidgetItem(QString::number(currentSchet[i].count)));
        ui->tableAccount->setItem(i, 4, new QTableWidgetItem(QString::number(currentSchet[i].price,'g', 10)));
    }
    ui->tableAccount->resizeColumnsToContents();
    ui->tableAccount->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::on_checkBoxAllDocumentsPrihod_stateChanged(int arg1)
{
    if(arg1)
    {
        ui->dateEditBeginPrihod->setEnabled(false);
        ui->dateEditEndPrihod->setEnabled(false);
    }else{
        ui->dateEditBeginPrihod->setEnabled(true);
        ui->dateEditEndPrihod->setEnabled(true);
    }
}

void MainWindow::on_checkBoxAllDocumentsRashod_stateChanged(int arg1)
{
    if(arg1)
    {
        ui->dateEditBeginRashod->setEnabled(false);
        ui->dateEditEndRashod->setEnabled(false);
    }else{
        ui->dateEditBeginRashod->setEnabled(true);
        ui->dateEditEndRashod->setEnabled(true);
    }
}
