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

    // Подключение к БД
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

    //Забираем данные о нарядах из БД
    QString textQuery = "SELECT * FROM orders";
    query.exec(textQuery);
    qDebug() << query.lastError();

    while(query.next())
    {
        order tmp;
        tmp.name = query.value(1).toString();
        tmp.numberOutput = query.value(2).toString();
        tmp.dateOutput = QDate::fromString(query.value(3).toString(), "dd.MM.yyyy");
        tmp.numberInput = query.value(4).toString();
        tmp.dateInput = QDate::fromString(query.value(5).toString(), "dd.MM.yyyy");
        tmp.chief = query.value(6).toString();
        tmp.oficier = query.value(7).toString();
        tmp.sendType = query.value(8).toString();
        tmp.base = query.value(9).toString();
        tmp.expirationDate = QDate::fromString(query.value(10).toString(), "dd.MM.yyyy");
        tmp.tlg = query.value(11).toString();
        tmp.typeProduct = query.value(12).toString();
        tmp.completion = query.value(13).toInt();

        //Забираем отправителей наряда из БД
        QString id_orders = query.value(0).toString();
        QSqlQuery query2;
        query2.exec("SELECT sender FROM orderSenders WHERE id_orders = " + id_orders);
        while(query2.next())
        {
            tmp.senders.push_back(query2.value(0).toString());
        }

        //Забираем получателей наряда из БД
        query2.exec("SELECT recipient FROM recipientsOrder WHERE id_orders = " + id_orders);
        while(query2.next())
        {
            tmp.recipients.push_back(query2.value(0).toString());
        }

        //Забираем имущество наряда из БД
        query2.exec("SELECT * FROM product_orders WHERE id_orders = " + id_orders);
        while(query2.next())
        {
            product tmpProd;
            tmpProd.name = query2.value(1).toString();
            tmpProd.nomNumber = query2.value(2).toString();
            tmpProd.measure = query2.value(4).toString();
            tmpProd.count = query2.value(5).toDouble();
            tmpProd.factoryNumber = query2.value(6).toString();
            tmpProd.category = query2.value(7).toInt();
            tmpProd.releaseDate = query2.value(8).toDate();
            tmp.items.push_back(tmpProd);
        }
        orders.push_back(tmp);
    }

    //Добавляем в таблицу данные о нарядах
    ui->tableWidgetOrders->setRowCount(orders.size());
    for(int i = 0; i < orders.size(); ++i)
    {
        ui->tableWidgetOrders->setItem(i, 0, new QTableWidgetItem(orders[i].name));
        ui->tableWidgetOrders->setItem(i, 1, new QTableWidgetItem(orders[i].numberOutput));
        ui->tableWidgetOrders->setItem(i, 2, new QTableWidgetItem(orders[i].dateOutput.toString("dd.MM.yyyy")));
        ui->tableWidgetOrders->setItem(i, 3, new QTableWidgetItem(orders[i].numberInput + " от " + orders[i].dateInput.toString("dd.MM.yyyy")));
        ui->tableWidgetOrders->setItem(i, 4, new QTableWidgetItem(orders[i].expirationDate.toString("dd.MM.yyyy")));
        ui->tableWidgetOrders->setItem(i, 5, new QTableWidgetItem(orders[i].senders[0]));
        ui->tableWidgetOrders->setItem(i, 6, new QTableWidgetItem(orders[i].recipients[0]));
        ui->tableWidgetOrders->setItem(i, 7, new QTableWidgetItem(orders[i].base));
        ui->tableWidgetOrders->setItem(i, 8, new QTableWidgetItem(QString::number(orders[i].completion)));

        QComboBox *comboOrderProducts = new QComboBox;
        for(int j = 0; j < orders[i].items.size(); ++j)
        {
            QString productName = orders[i].items[j].nomNumber + " | " + orders[i].items[j].name;
            if(productName.size() > 60)
            {
                productName.resize(60);
                productName += " ...";
            }
            comboOrderProducts->addItem(productName);
        }
        ui->tableWidgetOrders->setCellWidget(i, 9, comboOrderProducts);
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

        //ui->tableWidgetOrders->setRowCount(orders.size());
    }
}

//Открыть вкладку "Организации"
void MainWindow::on_organisations_triggered()
{
    if (check_available_tab("Организации"))
    {
        new_tab("Организации");
        //Переключение фокуса на вкладку с организациями
        ui->tabWidgetOrganisations->setCurrentIndex(0);
        importOrgsAndHumans();
    }
}

//Информация о текущей организации
void MainWindow::on_infoAboutOrg_triggered()
{

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

//Нажатие кнопки "Показать все приходные документы
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

//Нажатие кнопки "Показать все расходные документы
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

//Нажатие чекбокса "Показать распоряжения за все время"
void MainWindow::on_checkBoxAllDocumentsPrihod_2_stateChanged(int arg1)
{
    if(arg1)
    {
        ui->dateEditBeginOrder->setEnabled(false);
        ui->dateEditEndOrder->setEnabled(false);
    }else{
        ui->dateEditBeginOrder->setEnabled(true);
        ui->dateEditEndOrder->setEnabled(true);
    }
}
void MainWindow::on_pushButtonNextPrihod_2_clicked()
{
    order temp;
    temp.name = ui->comboBoxTypeOrder->currentText();
    temp.base = ui->lineEditBaseOrder->text();
    temp.numberOutput = ui->lineEditOutputOrder->text();
    temp.numberInput = ui->lineEditInputOrder->text();
    temp.dateOutput = ui->dateOutputOrder->date();
    temp.dateInput = ui->dateInputOrder->date();
    temp.completion = 0;
    //temp.senders.push_back(ui->comboBoxSenderOrder->currentText());
    //temp.recipients.push_back(ui->comboBoxRecipientOrder->currentText());

}

//Выбор файлов фото или сканов
void MainWindow::add_scans()
{
    tempScans.clear();
    QStringList scansList = QFileDialog::getOpenFileNames(this, "Выберите сканы или фотографии документа...", "", "*.png, *.jpeg");
    for (int i = 0; i < scansList.size(); ++i)
    {
        QImage temp(scansList[i]);
        tempScans.push_back(temp);
    }
}

//Нажатие кнопки "Добавить сканы или фото наряда"
void MainWindow::on_pushButtonScanOrder_clicked()
{
    add_scans();
    currentScanIndex = 0;
    if(!tempScans.isEmpty())
    {
        ui->scanOrder->setPixmap(QPixmap::fromImage(tempScans[0]));
    }
}

//Нажатие стрелки "Вправо"
void MainWindow::on_pushButtonRightOrder_clicked()
{
    if(currentScanIndex == tempScans.size())
    {
        currentScanIndex = 0;
    }
    ui->scanOrder->setPixmap(QPixmap::fromImage((tempScans[currentScanIndex])));
    currentScanIndex ++;
}

//Нажатие кнопки "Влево"
void MainWindow::on_pushButtonLeftOrder_clicked()
{
    if(currentScanIndex == -1)
    {
        currentScanIndex = tempScans.size() - 1;
    }
    ui->scanOrder->setPixmap(QPixmap::fromImage((tempScans[currentScanIndex])));
    currentScanIndex --;
}


//Нажатие кнопки "Добавить сотрудника"
void MainWindow::on_buttonAddWorker_clicked()
{
    if(ui->comboboxEditDepart->currentText() == "")
    {
        QMessageBox msg;
        msg.setWindowTitle("Внимание!");
        msg.setText("Выберите подразделение или добавьте новое!");
        msg.exec();
        ui->comboboxEditDepart->setToolTip("Введите наименование нового подразделения!");
        ui->comboboxEditDepart->setFocus();
    }else{
        ui->comboboxEditWorker->setCurrentIndex(-1);
        ui->addWorkerPosition->setReadOnly(false);
        ui->addWorkerPosition->setPlaceholderText("Введите должность сотрудника...");
        ui->addWorkerPosition->setFocus();

        ui->addWorkerRank->setReadOnly(false);
        ui->addWorkerRank->setPlaceholderText("Введите воинское звание сотрудника (при наличии)...");

        ui->addWorkerSecName->setReadOnly(false);
        ui->addWorkerSecName->setPlaceholderText("Введите фамилию сотрудника...");

        ui->addWorkerName->setReadOnly(false);
        ui->addWorkerName->setPlaceholderText("Введите имя сотрудника...");

        ui->addWorkerMidName->setReadOnly(false);
        ui->addWorkerMidName->setPlaceholderText("Введите отчество сотрудника...");

        ui->addWorkerPhonenumber->setReadOnly(false);
        ui->addWorkerPhonenumber->setPlaceholderText("Введите номер телефона сотрудника...");

        ui->addWorkerEmail->setReadOnly(false);
        ui->addWorkerEmail->setPlaceholderText("Введите электронную почту сотрудника...");

    }
}

// Нажатие кнопки "Добавить подразделение"
void MainWindow::on_buttonAddDepart_clicked()
{
    QString temp = ui->comboboxEditDepart->currentText();
    if(temp == "")
    {
        QMessageBox msg;
        msg.setWindowTitle("Внимание!");
        msg.setText("Выберите подразделение или добавьте новое!");
        msg.exec();
        ui->comboboxEditDepart->setToolTip("Введите наименование нового подразделения!");
        ui->comboboxEditDepart->setFocus();
    }else{
        if(ui->comboboxEditDepart->findText(temp) != -1)
        {
            QMessageBox msg;
            msg.setWindowTitle("Внимание!");
            msg.setText("Данное подразделение уже существует в этой организации! Добавьте новое или выберите другое подразделение из списка!");
            msg.exec();
        }else {
            ui->comboboxEditDepart->addItem(temp);
            QVector <human> tempHumans;
            tempDeps.insert(temp, tempHumans); //Добавление подразделения и пустого списка сотрудников во временное хранилище
            ui->statusBar->showMessage("Подразделение \"" + temp + "\" добавлено.");

            //Удаление с экрана данных сотрудника
            ui->addWorkerPosition->clear();
            ui->addWorkerRank->clear();
            ui->addWorkerSecName->clear();
            ui->addWorkerName->clear();
            ui->addWorkerMidName->clear();
            ui->addWorkerPhonenumber->clear();
            ui->addWorkerEmail->clear();
        }
    }
}

// Нажатие кнопки "Удалить подразделение"
void MainWindow::on_buttonRemoveDepart_clicked()
{
    QString currentDep = ui->comboboxEditDepart->currentText();
    if(currentDep == "")
    {
        QMessageBox msg;
        msg.setWindowTitle("Внимание!");
        msg.setText("Выберите подразделение или добавьте новое!");
        msg.exec();
    }else{
        tempDeps.remove(currentDep);
        ui->comboboxEditDepart->removeItem(ui->comboboxEditDepart->currentIndex());
        ui->statusBar->showMessage("Подразделение \"" + currentDep + "\" удалено.");
        ui->comboboxEditWorker->clear();
        for(int i = 0; i < tempDeps[currentDep].size(); ++i)
        {
            ui->comboboxEditWorker->addItem(tempDeps[currentDep][i].position + " - " + tempDeps[currentDep][i].rank + " " + tempDeps[currentDep][i].FIO);
        }
        //Удаление с экрана данных сотрудника
        ui->addWorkerPosition->clear();
        ui->addWorkerRank->clear();
        ui->addWorkerSecName->clear();
        ui->addWorkerName->clear();
        ui->addWorkerMidName->clear();
        ui->addWorkerPhonenumber->clear();
        ui->addWorkerEmail->clear();
    }
}

// Нажатие кнопки "Сбросить данные сотрудника"
void MainWindow::on_buttonResetWorker_clicked()
{
    ui->addWorkerPosition->clear();

    ui->addWorkerRank->clear();

    ui->addWorkerSecName->clear();

    ui->addWorkerName->clear();

    ui->addWorkerMidName->clear();

    ui->addWorkerPhonenumber->clear();

    ui->addWorkerEmail->clear();
}

//Нажатие кнопки "Сохранить сотрудника"
void MainWindow::on_buttonSaveWorker_clicked()
{
    human tempHuman;
    tempHuman.position = ui->addWorkerPosition->text();
    ui->addWorkerPosition->clear();
    ui->addWorkerPosition->setReadOnly(true);

    tempHuman.rank = ui->addWorkerRank->text();
    ui->addWorkerRank->clear();
    ui->addWorkerRank->setReadOnly(true);

    tempHuman.secondName = ui->addWorkerSecName->text();
    ui->addWorkerSecName->clear();
    ui->addWorkerSecName->setReadOnly(true);

    tempHuman.firstName = ui->addWorkerName->text();
    ui->addWorkerName->clear();
    ui->addWorkerName->setReadOnly(true);

    tempHuman.middleName = ui->addWorkerMidName->text();
    ui->addWorkerMidName->clear();
    ui->addWorkerMidName->setReadOnly(true);

    tempHuman.phoneNumber = ui->addWorkerPhonenumber->text();
    ui->addWorkerPhonenumber->clear();
    ui->addWorkerPhonenumber->setReadOnly(true);

    tempHuman.email = ui->addWorkerEmail->text();
    ui->addWorkerEmail->clear();
    ui->addWorkerEmail->setReadOnly(true);

    tempHuman.FIO = tempHuman.secondName + " " + tempHuman.firstName [0] + ". " + tempHuman.middleName[0] + ".";

    if(ui->comboboxEditWorker->currentIndex() == -1)
    {
        tempDeps[ui->comboboxEditDepart->currentText()].push_back(tempHuman);
        ui->comboboxEditWorker->addItem(tempHuman.position + " - " + tempHuman.rank + " " + tempHuman.FIO);
        ui->statusBar->showMessage("Сотрудник \"" + tempHuman.FIO + "\" добавлен.");
    }else{
        tempDeps[ui->comboboxEditDepart->currentText()][ui->comboboxEditWorker->currentIndex()] = tempHuman;
        ui->comboboxEditWorker->setCurrentText(tempHuman.position + " - " + tempHuman.rank + " " + tempHuman.FIO);
        ui->statusBar->showMessage("Данные сотрудника \"" + tempHuman.FIO + "\" изменен.");
    }
}

//Загрузка сотрудников при выборе подразделения (изменение организаций)
void MainWindow::on_comboboxEditDepart_activated(const QString &arg1)
{
    ui->comboboxEditWorker->clear();
    for(int i = 0; i < tempDeps[arg1].size(); ++i)
    {
        ui->comboboxEditWorker->addItem(tempDeps[arg1][i].position + " - " + tempDeps[arg1][i].rank + " " + tempDeps[arg1][i].FIO);
    }
}

//Нажатие кнопки "Удалить выбранного сотрудника"
void MainWindow::on_buttonRemoveWorker_clicked()
{
    int currentWorkerIndex = ui->comboboxEditWorker->currentIndex();
    ui->comboboxEditWorker->removeItem(currentWorkerIndex);
    tempDeps[ui->comboboxEditDepart->currentText()].remove(currentWorkerIndex);

    //Удаление с экрана данных сотрудника
    ui->addWorkerPosition->clear();
    ui->addWorkerRank->clear();
    ui->addWorkerSecName->clear();
    ui->addWorkerName->clear();
    ui->addWorkerMidName->clear();
    ui->addWorkerPhonenumber->clear();
    ui->addWorkerEmail->clear();
}

// Загрузка данных о сотруднике при выборе его из списка (изменение организаций)
void MainWindow::on_comboboxEditWorker_activated(int index)
{
    ui->addWorkerPosition->setText(tempDeps[ui->comboboxEditDepart->currentText()][index].position);

    ui->addWorkerRank->setText(tempDeps[ui->comboboxEditDepart->currentText()][index].rank);

    ui->addWorkerSecName->setText(tempDeps[ui->comboboxEditDepart->currentText()][index].secondName);

    ui->addWorkerName->setText(tempDeps[ui->comboboxEditDepart->currentText()][index].firstName);

    ui->addWorkerMidName->setText(tempDeps[ui->comboboxEditDepart->currentText()][index].middleName);

    ui->addWorkerPhonenumber->setText(tempDeps[ui->comboboxEditDepart->currentText()][index].phoneNumber);

    ui->addWorkerEmail->setText(tempDeps[ui->comboboxEditDepart->currentText()][index].email);

    ui->addWorkerPosition->setReadOnly(false);
    ui->addWorkerPosition->setFocus();

    ui->addWorkerRank->setReadOnly(false);

    ui->addWorkerSecName->setReadOnly(false);

    ui->addWorkerName->setReadOnly(false);

    ui->addWorkerMidName->setReadOnly(false);

    ui->addWorkerPhonenumber->setReadOnly(false);

    ui->addWorkerEmail->setReadOnly(false);
}

//Нажатие кнопки "Сохранить организацию"
void MainWindow::on_buttonSaveOrg_clicked()
{
    organisation tempOrg;
    tempOrg.name = ui->addNameOrg->text();
    tempOrg.city = ui->addCityOrg->text();
    tempOrg.index = ui->addPostcodeOrg->text();
    tempOrg.inn = ui->addInnOrg->text();
    tempOrg.phoneNumber = ui->addPhonenumberOrg->text();
    tempOrg.vpMORF = ui->addVpMORFOrg->text();

    organisations.insert(tempOrg.name, tempDeps);
    orgsContainer.insert(tempOrg.name, tempOrg);

    QSqlQuery query;

    QString queryText = "INSERT INTO organisations (name, inn, phoneNumber, city, postCode, vpMORF) VALUES (\""
            + tempOrg.name + "\", \"" + tempOrg.inn + "\", \"" + tempOrg.phoneNumber + "\", \"" + tempOrg.city + "\", \"" + tempOrg.index + "\", \"" + tempOrg.vpMORF + "\")";
    query.exec(queryText);
    qDebug() << query.lastError();

    queryText = "SELECT id FROM organisations WHERE name = \"" + tempOrg.name + "\"";
    query.exec(queryText);
    qDebug() << query.lastError();

    query.next();  // Добавить отлов возможных ошибок
    int idOrg = query.value(0).toInt();

    QMapIterator <QString, QVector <human> > it(tempDeps);
    while(it.hasNext())
    {
        it.next();
        QString depName = it.key();
        queryText = "INSERT INTO departments_of_org (name, id_organisation) VALUES (\""
                + depName + "\", " + QString::number(idOrg) + ")";
        query.exec(queryText);
        qDebug() << query.lastError();

        queryText = "SELECT id FROM departments_of_org WHERE name = \"" + depName + "\"";
        query.exec(queryText);
        qDebug() << query.lastError();

        query.next();  // Добавить отлов возможных ошибок
        int idDep = query.value(0).toInt();

        for(int i = 0; i < it.value().size(); ++i)
        {
            queryText = "INSERT INTO humans (position, FIO, firstName, secondName, middleName, rank, phoneNumber, email, id_department) VALUES (\"" +
                    it.value()[i].position + "\", \"" + it.value()[i].FIO + "\", \"" + it.value()[i].firstName + "\", \"" + it.value()[i].secondName + "\", \"" +
                    it.value()[i].middleName + "\", \"" + it.value()[i].rank + "\", \"" + it.value()[i].phoneNumber + "\", \"" + it.value()[i].email + "\", " +
                    QString::number(idDep) + ")";
            query.exec(queryText);
        }
    }

    tempDeps.clear();
    //Переключение фокуса на вкладку с организациями
    ui->tabWidgetOrganisations->setCurrentIndex(0);
}

//Нажатие кнопки "Отмена" на вкладке добавления организации
void MainWindow::on_buttonCancelAddOrg_clicked()
{
    //Удаление с экрана данных о сотруднике
    ui->addWorkerPosition->clear();
    ui->addWorkerRank->clear();
    ui->addWorkerSecName->clear();
    ui->addWorkerName->clear();
    ui->addWorkerMidName->clear();
    ui->addWorkerPhonenumber->clear();
    ui->addWorkerEmail->clear();

    //Удаление с экрана данных об организации
    ui->addNameOrg->clear();
    ui->addCityOrg->clear();
    ui->addPostcodeOrg->clear();
    ui->addInnOrg->clear();
    ui->addPhonenumberOrg->clear();
    ui->addVpMORFOrg->clear();

    //Переключение фокуса на вкладку с организациями
    ui->tabWidgetOrganisations->setCurrentIndex(0);
}

//Импорт организаций и сотрудников
void MainWindow::importOrgsAndHumans()
{
    QSqlQuery query, query2, query3;
    QString textQuery = "SELECT * FROM organisations";
    query.exec(textQuery);
    qDebug() << query.lastError();

    while(query.next())
    {
        organisation tempOrg;
        tempOrg.name = query.value(1).toString();
        tempOrg.inn = query.value(2).toString();
        tempOrg.phoneNumber = query.value(3).toString();
        tempOrg.city = query.value(4).toString();
        tempOrg.index = query.value(5).toString();
        tempOrg.vpMORF = query.value(6).toString();
        tempOrg.checkMyOrg = query.value(7).toBool();
        orgsContainer.insert(tempOrg.name, tempOrg);

        textQuery = "SELECT * FROM departments_of_org WHERE id_organisation = " + query.value(0).toString();
        query2.exec(textQuery);
        qDebug() << query2.lastError();
        while(query2.next())
        {
            QString nameDepart = query2.value(1).toString();

            textQuery = "SELECT * FROM humans WHERE id_department = " + query2.value(0).toString();
            query3.exec(textQuery);
            qDebug() << query3.lastError();
            QVector <human> tmpHumans;
            while(query3.next())
            {
                human tmpHuman;
                tmpHuman.position = query3.value(1).toString();
                tmpHuman.FIO = query3.value(2).toString();
                tmpHuman.firstName = query3.value(3).toString();
                tmpHuman.secondName = query3.value(4).toString();
                tmpHuman.middleName = query3.value(5).toString();
                tmpHuman.rank = query3.value(6).toString();
                tmpHuman.phoneNumber = query3.value(7).toString();
                tmpHuman.email = query3.value(8).toString();
                tmpHumans.push_back(tmpHuman);
            }
            tempDeps.insert(nameDepart, tmpHumans);
        }
        organisations.insert(tempOrg.name, tempDeps);
        tempDeps.clear();
    }

    QMapIterator <QString, QMap <QString, QVector <human> > > it (organisations);

    while(it.hasNext())
    {
        it.next();
        ui->comboboxSelectOrg->addItem(it.key());
    }

    if(orgsContainer[it.key()].checkMyOrg)
    {
        ui->nameOrg->setText(orgsContainer[ui->comboboxSelectOrg->currentText()].name + " - моя организация");
    }else{
        ui->nameOrg->setText(orgsContainer[ui->comboboxSelectOrg->currentText()].name);
    }
    ui->cityOrg->setText(orgsContainer[ui->comboboxSelectOrg->currentText()].city);
    ui->cityOrg->setReadOnly(true);
    ui->postcodeOrg->setText(orgsContainer[ui->comboboxSelectOrg->currentText()].index);
    ui->postcodeOrg->setReadOnly(true);
    ui->innOrg->setText(orgsContainer[ui->comboboxSelectOrg->currentText()].inn);
    ui->innOrg->setReadOnly(true);
    ui->phonenumberOrg->setText(orgsContainer[ui->comboboxSelectOrg->currentText()].phoneNumber);
    ui->phonenumberOrg->setReadOnly(true);
    ui->vpMORFOrg->setText(orgsContainer[ui->comboboxSelectOrg->currentText()].vpMORF);
    ui->vpMORFOrg->setReadOnly(true);

    QMapIterator <QString, QVector <human> > iter(organisations[ui->comboboxSelectOrg->currentText()]);

    while(iter.hasNext())
    {
        iter.next();
        ui->comboboxSelectDepart->addItem(iter.key());
    }

    QVectorIterator <human> vectIT (organisations[ui->comboboxSelectOrg->currentText()][ui->comboboxSelectDepart->currentText()]);

    while(vectIT.hasNext())
    {
        human hum = vectIT.next();
        ui->comboboxSelectWorker->addItem(hum.position + " - " + hum.rank + " " + hum.FIO);
    }
}

//Загрузка сотрудников при выборе подразделения (просмотр организаций)
void MainWindow::on_comboboxSelectDepart_activated(const QString &arg1)
{
    ui->comboboxSelectWorker->clear();
    for(int i = 0; i < organisations[ui->comboboxSelectOrg->currentText()][arg1].size(); ++i)
    {
        ui->comboboxSelectWorker->addItem(organisations[ui->comboboxSelectOrg->currentText()][arg1][i].position + " - " + organisations[ui->comboboxSelectOrg->currentText()][arg1][i].rank + " " + organisations[ui->comboboxSelectOrg->currentText()][arg1][i].FIO);
    }
}

// Загрузка данных о сотруднике при выборе его из списка (просмотр организаций)
void MainWindow::on_comboboxSelectWorker_activated(int index)
{

    ui->workerPosition->setText(organisations[ui->comboboxSelectOrg->currentText()][ui->comboboxSelectDepart->currentText()][index].position);

    ui->workerRank->setText(organisations[ui->comboboxSelectOrg->currentText()][ui->comboboxSelectDepart->currentText()][index].rank);

    ui->workerSecName->setText(organisations[ui->comboboxSelectOrg->currentText()][ui->comboboxSelectDepart->currentText()][index].secondName);

    ui->workerName->setText(organisations[ui->comboboxSelectOrg->currentText()][ui->comboboxSelectDepart->currentText()][index].firstName);

    ui->workerMidName->setText(organisations[ui->comboboxSelectOrg->currentText()][ui->comboboxSelectDepart->currentText()][index].middleName);

    ui->workerPhoneNumber->setText(organisations[ui->comboboxSelectOrg->currentText()][ui->comboboxSelectDepart->currentText()][index].phoneNumber);

    ui->workerEmail->setText(organisations[ui->comboboxSelectOrg->currentText()][ui->comboboxSelectDepart->currentText()][index].email);

    ui->workerPosition->setReadOnly(true);

    ui->workerRank->setReadOnly(true);

    ui->workerSecName->setReadOnly(true);

    ui->workerName->setReadOnly(true);

    ui->workerMidName->setReadOnly(true);

    ui->workerPhoneNumber->setReadOnly(true);

    ui->workerEmail->setReadOnly(true);
}

//Нажатие кнопки изменить организацию
void MainWindow::on_buttonChangeOrg_clicked()
{
    ui->tabWidgetOrganisations->setTabText(1, "Изменить данные организации");
    ui->tabWidgetOrganisations->setCurrentIndex(1);
}

//Нажатие кнопки "Далее" на вкладке регистрации распоряжения (Основная информация)
void MainWindow::on_pushButtonNextOrder_clicked()
{
    tempOrder.clear();

    tempOrder.name = ui->comboBoxTypeOrder->currentText();
    tempOrder.numberOutput = ui->lineEditOutputOrder->text();
    tempOrder.dateOutput = ui->dateOutputOrder->date();
    tempOrder.numberInput = ui->lineEditInputOrder->text();
    tempOrder.dateInput = ui->dateInputOrder->date();
    tempOrder.expirationDate = ui->dateEditExpiration->date();
    tempOrder.base = ui->lineEditBaseOrder->text();
    tempOrder.senders.push_back(ui->comboBoxSenderOrder->currentText());
    tempOrder.recipients.push_back(ui->comboBoxRecipientOrder->currentText());
    tempOrder.scans = tempScans;
    tempOrder.sendType = ui->comboBoxSendType->currentText();
    ui->tabWidgetRegOrder->setCurrentIndex(1);
}

//Нажатие кнопки "Назад" на вкладке регистрации распоряжения (Перечень МЦ)
void MainWindow::on_pushButtonBackOrder_clicked()
{
    ui->tabWidgetRegOrder->setCurrentIndex(0);
}

//Нажатие кнопки "Отмена" на вкладке регистрации распоряжения (Основная информация)
void MainWindow::on_pushButtonCancelOrder_clicked()
{
    ui->comboBoxTypeOrder->clear();
    ui->lineEditOutputOrder->clear();
    ui->dateOutputOrder->setDate(QDate(2000, 1, 1));
    ui->lineEditInputOrder->clear();
    ui->dateInputOrder->setDate(QDate(2000, 1, 1));
    ui->dateEditExpiration->setDate(QDate(2000, 1, 1));
    ui->lineEditBaseOrder->clear();
    ui->comboBoxSenderOrder->clear();
    ui->comboBoxRecipientOrder->clear();

    ui->tabWidgetRegOrder->setCurrentIndex(0);
    ui->tabWidgetOrders->setCurrentIndex(1);
}

//Нажатие кнопки "Отмена" на вкладке регистрации распоряжения (Перечень МЦ)
void MainWindow::on_pushButtonCancelOderMC_clicked()
{
    ui->comboBoxTypeOrder->clear();
    ui->lineEditOutputOrder->clear();
    ui->dateOutputOrder->setDate(QDate(2000, 1, 1));
    ui->lineEditInputOrder->clear();
    ui->dateInputOrder->setDate(QDate(2000, 1, 1));
    ui->dateEditExpiration->setDate(QDate(2000, 1, 1));
    ui->lineEditBaseOrder->clear();
    ui->comboBoxSenderOrder->clear();
    ui->comboBoxRecipientOrder->clear();
    ui->tableWidgetProductsOrder->clearContents();
    ui->tableWidgetProductsOrder->setRowCount(1);

    ui->tabWidgetRegOrder->setCurrentIndex(0);
    ui->tabWidgetOrders->setCurrentIndex(1);
}

//Нажатие кнопки "+" на вкладке Перечень МЦ при регистрации распоряжения
void MainWindow::on_pushButtonAddLineOrder_clicked()
{
    int currentRow = ui->tableWidgetProductsOrder->rowCount();
    ui->tableWidgetProductsOrder->setRowCount(currentRow + 1);

    ui->tableWidgetProductsOrder->setItem(currentRow, 0, new QTableWidgetItem(""));
    ui->tableWidgetProductsOrder->setItem(currentRow, 1, new QTableWidgetItem(""));
    ui->tableWidgetProductsOrder->setItem(currentRow, 2, new QTableWidgetItem(""));
    ui->tableWidgetProductsOrder->setItem(currentRow, 3, new QTableWidgetItem(""));
    ui->tableWidgetProductsOrder->setItem(currentRow, 4, new QTableWidgetItem(""));
    ui->tableWidgetProductsOrder->setItem(currentRow, 5, new QTableWidgetItem(""));
    ui->tableWidgetProductsOrder->setItem(currentRow, 6, new QTableWidgetItem(""));

    //Добавление в таблицу ComboBox с категориями
    /*QComboBox *comboCategory = new QComboBox;
    comboCategory->addItem("I");
    comboCategory->addItem("II");
    comboCategory->addItem("III");
    comboCategory->addItem("IV");
    comboCategory->addItem("V");
    ui->tableWidgetProductsOrder->setCellWidget(currentRow, 4, comboCategory);
    */

    /*QSqlQuery query;
    query.exec("SELECT nomkl_nom, name, ed_izm FROM spr_ati");
    qDebug() << query.lastError();

    QMap <QString, QPair <QString, QString>> tempProducts;
    QComboBox *comboNomNumb = new QComboBox;

    while(query.next())
    {
        tempProducts.insert(query.value(0).toString(), qMakePair(query.value(1).toString(), query.value(2).toString()));
        comboNomNumb->addItem(query.value(0).toString() + " | " + query.value(1).toString());
    }
*/
    //comboNomNumb->setcu
    //ui->tableWidgetProductsOrder->setCellWidget(currentRow, 1, comboNomNumb);

}

//Нажатие кнопки "Вниз" на вкладке распоряжения (Перечень МЦ)
void MainWindow::on_pushButtonDownOrder_clicked()
{
    int row = ui->tableWidgetProductsOrder->currentRow(), rowCount = ui->tableWidgetProductsOrder->rowCount();

    if(rowCount > 0)
    {
        if(row < rowCount - 1)
        {
            //ui->tableWidgetProductsOrder->setCurrentCell(row + 1, column);
            ui->tableWidgetProductsOrder->selectRow(row + 1);
        }else{
            //ui->tableWidgetProductsOrder->setCurrentCell(0, column);
            ui->tableWidgetProductsOrder->selectRow(0);
        }
    }
}

//Нажатие кнопки "Вверх" на вкладке распоржения (Перечень МЦ)
void MainWindow::on_pushButtonUpPOrder_clicked()
{
    int row = ui->tableWidgetProductsOrder->currentRow(), rowCount = ui->tableWidgetProductsOrder->rowCount();

    if(rowCount > 0)
    {
        if(row > 0)
        {
            //ui->tableWidgetProductsOrder->setCurrentCell(row + 1, column);
            ui->tableWidgetProductsOrder->selectRow(row - 1);
        }else{
            //ui->tableWidgetProductsOrder->setCurrentCell(0, column);
            ui->tableWidgetProductsOrder->selectRow(rowCount - 1);
        }
    }
}

//Нажатие кнопки "Удалить" на вкладке распоряжения (Перечень МЦ)
void MainWindow::on_pushButtonRemoveLineOrder_clicked()
{
    int row = ui->tableWidgetProductsOrder->currentRow();
    if(row > -1)
    {
        ui->tableWidgetProductsOrder->removeRow(row);
        ui->tableWidgetProductsOrder->selectRow(ui->tableWidgetProductsOrder->currentRow());
    }
}

//Нажатие кнопки "Зарегистрировать" на вкладке распоряжения (Перечень МЦ)
void MainWindow::on_pushButtonDoneOrder_clicked()
{
    QVector <product> tempProdContainer;

    for(int i = 0; i < ui->tableWidgetProductsOrder->rowCount(); ++i)
    {
        product tempProd;

        tempProd.name = ui->tableWidgetProductsOrder->item(i, 0)->text();
        if(ui->tableWidgetProductsOrder->item(i, 1)->text() != "")
        {
            tempProd.nomNumber = ui->tableWidgetProductsOrder->item(i, 1)->text();
        }else{
            tempProd.nomNumber = "б/н";
        }

        tempProd.factoryNumber = ui->tableWidgetProductsOrder->item(i, 2)->text();

        if(ui->tableWidgetProductsOrder->item(i, 3)->text() != "")
        {
            tempProd.releaseDate = QDate::fromString(ui->tableWidgetProductsOrder->item(i, 3)->text(),"dd.MM.yyyy");
        }
        /*QString category = ui->tableWidgetProductsOrder->item(i, 4);

        QComboBox *tempCombo = ui->tableWidgetProductsOrder->item(i,4);
        if(category == "I") tempProd.category = 1;
        if(category == "II") tempProd.category = 2;
        if(category == "III") tempProd.category = 3;
        if(category == "IV") tempProd.category = 4;
        if(category == "V") tempProd.category = 5;
        */
        tempProd.category = ui->tableWidgetProductsOrder->item(i, 4)->text().toInt();
        tempProd.measure = ui->tableWidgetProductsOrder->item(i, 5)->text();
        tempProd.count =  ui->tableWidgetProductsOrder->item(i, 6)->text().toDouble();
        tempProd.orderNumber = tempOrder.name + " №" + tempOrder.numberOutput + " от " + tempOrder.dateOutput.toString("dd.MM.yyyy"); // ????

        tempProdContainer.push_back(tempProd);
    }
    tempOrder.items = tempProdContainer;
    orders.push_back(tempOrder);

    //Добавление распоряжения в БД
    QSqlQuery query;
    QString textQuery = "INSERT INTO orders (name, numberOutput, dateOutput, numberInput, dateInput, base, expirationDate, completion, sendType) VALUES (\"" +
            tempOrder.name + "\", \"" + tempOrder.numberOutput + "\", \"" + tempOrder.dateOutput.toString("dd.MM.yyyy") + "\", \"" + tempOrder.numberInput + "\", \"" +
            tempOrder.dateInput.toString("dd.MM.yyyy") + "\", \"" + tempOrder.base + "\", \"" + tempOrder.expirationDate.toString("dd.MM.yyyy") + "\", " + QString::number(tempOrder.completion) + ", \"" + tempOrder.sendType + "\")";
    query.exec(textQuery);
    qDebug() << query.lastError();

    //Забираем из БД id распоряжения
    query.exec("SELECT id FROM orders WHERE numberOutput = \"" + tempOrder.numberOutput + "\" AND dateOutput = \"" + tempOrder.dateOutput.toString("dd.MM.yyyy") + "\"");
    qDebug() << query.lastError();
    query.next();
    qDebug() << query.lastError();
    QString id_order = query.value(0).toString();
    //Добавление Отправителей в наряд
    for(int i = 0; i < tempOrder.senders.size(); ++i)
    {
        textQuery = "INSERT INTO orderSenders (sender, id_orders) VALUES (\"" + tempOrder.senders[i] + "\", " + id_order + ")";
        query.exec(textQuery);
        qDebug() << query.lastError();
    }

    //Добавление получателей в наряд
    for(int i = 0; i < tempOrder.recipients.size(); ++i)
    {
        textQuery = "INSERT INTO recipientsOrder (recipient, id_orders) VALUES (\"" + tempOrder.recipients[i] + "\", " + id_order + ")";
        query.exec(textQuery);
        qDebug() << query.lastError();
    }

    for(int i = 0; i < tempOrder.items.size(); ++i)
    {
        textQuery = "INSERT INTO product_orders (name, nom_numb, measure, count, factoryNumber, category, releaseDate, id_orders) VALUES (\"" + tempOrder.items[i].name + "\", \""
                + tempOrder.items[i].nomNumber + "\", \"" + tempOrder.items[i].measure + "\", \"" + QString::number(tempOrder.items[i].count, 'g', 2) + "\", \""
                + tempOrder.items[i].factoryNumber + "\", \"" + QString::number(tempOrder.items[i].category) + "\", \"" + tempOrder.items[i].releaseDate.toString("dd.MM.yyyy") + "\", " + id_order + ")";
        query.exec(textQuery);
        qDebug() << query.lastError();
        qDebug() << textQuery;
    }
    //Обновление списка зарегистрированных нарядов
    ui->tableWidgetOrders->setRowCount(orders.size());
    for(int i = 0; i < orders.size(); ++i)
    {
        ui->tableWidgetOrders->setItem(i, 0, new QTableWidgetItem(orders[i].name));
        ui->tableWidgetOrders->setItem(i, 1, new QTableWidgetItem(orders[i].numberOutput));
        ui->tableWidgetOrders->setItem(i, 2, new QTableWidgetItem(orders[i].dateOutput.toString("dd.MM.yyyy")));
        ui->tableWidgetOrders->setItem(i, 3, new QTableWidgetItem(orders[i].numberInput + " от " + orders[i].dateInput.toString("dd.MM.yyyy")));
        ui->tableWidgetOrders->setItem(i, 4, new QTableWidgetItem(orders[i].expirationDate.toString("dd.MM.yyyy")));
        ui->tableWidgetOrders->setItem(i, 5, new QTableWidgetItem(orders[i].senders[0]));
        ui->tableWidgetOrders->setItem(i, 6, new QTableWidgetItem(orders[i].recipients[0]));
        ui->tableWidgetOrders->setItem(i, 7, new QTableWidgetItem(orders[i].base));
        ui->tableWidgetOrders->setItem(i, 8, new QTableWidgetItem(QString::number(orders[i].completion)));

        QComboBox *comboOrderProducts = new QComboBox;
        for(int j = 0; j < orders[i].items.size(); ++j)
        {
            QString productName = orders[i].items[j].nomNumber + " | " + orders[i].items[j].name;
            if(productName.size() > 60)
            {
                productName.resize(60);
                productName += " ...";
            }
            comboOrderProducts->addItem(productName);
        }
        ui->tableWidgetOrders->setCellWidget(i, 9, comboOrderProducts);
    }

    tempOrder.clear();
    ui->tabWidgetOrders->setCurrentIndex(1);

    //Очистка вкладки регистрации
    ui->lineEditOutputOrder->clear();
    ui->dateOutputOrder->setDate(QDate(2000, 1, 1));
    ui->lineEditInputOrder->clear();
    ui->dateInputOrder->setDate(QDate(2000, 1, 1));
    ui->dateEditExpiration->setDate(QDate(2000, 1, 1));
    ui->lineEditBaseOrder->clear();
    ui->tableWidgetProductsOrder->setRowCount(0);
}

