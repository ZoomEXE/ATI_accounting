#include "mainwindow.h"
#include "ui_mainwindow.h"

using namespace QXlsx;
//#include "header/xlsxdocument.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Добавление контекстного меню в таблицу распоряжений
    ui->tableWidgetOrders->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidgetOrders, SIGNAL(customContextMenuRequested(QPoint)), this,
              SLOT(showContextMenuOrders(QPoint)));
    Entry *ent = new Entry;
    connect(ent, SIGNAL(close_signal(QString, QString)), SLOT(close_entry(QString, QString)));
    ent->setModal(true);
    ent->exec();

    save_first_tabs();

    // Открытие главного окна
    if (check_available_tab("Главное окно"))
    {
        new_tab("Главное окно");
    }

    //Активация кнопок поиска по нажатию клавиши Enter
    ui->pushButtonFindPrihod_3->setShortcut(Qt::Key_Return);

    //ui->findInAtiInSkladButton->setShortcut(Qt::Key_Return);

    //ui->pushButtonFindPrihod_2->setShortcut(Qt::Key_Return);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Подключение к БД
    baseDB = QSqlDatabase::addDatabase("QSQLITE");
    baseDB.setDatabaseName("ats_db.db");

    if(!baseDB.open()) {
        qDebug() << baseDB.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }
    //baseDB = sdb;
    QSqlQuery query(baseDB);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Добавление баз данных
    QDir dir (QDir::currentPath());
    dir.setPath(dir.path() + "/DataBases");
    QFileInfoList infoList = dir.entryInfoList(QStringList() << "*.db" , QDir::Files);
    QStringList dbPaths;

    for(int i = 0; i < infoList.size(); ++i) {
        dbPaths.push_back(infoList.at(i).absoluteFilePath());
    }

    ui->lineEdit_2->setText(dbPaths.last());
    connectedDB(dbPaths);

    QMapIterator <QString, QSqlDatabase> iterDB(DataBases);
    ui->comboBoxOrderSender->addItem("Выберите отправителя");
    ui->comboBoxOrderRecipient->addItem("Выберите получателя");
    ui->selectVCh->addItem("Все в/ч");

    ui->comboBoxSenderOrder->addItem("Выберите отправителя");
    ui->comboBoxRecipientOrder->addItem("Выберите получателя");

    while(iterDB.hasNext())
    {
        iterDB.next();
        ui->comboBoxOrderSender->addItem(iterDB.key());
        //ui->comboBoxOrderRecipient->addItem(iterDB.key());

        ui->comboBoxSenderOrder->addItem(iterDB.key());
        //ui->comboBoxRecipientOrder->addItem(iterDB.key());
        //ui->comboBoxOrderRecipient->addItem(iterDB.key());

        ui->selectVCh->addItem(iterDB.key());

        //Добавление шаблона организации в список
        query.exec("SELECT name FROM organisations WHERE name = '" + iterDB.key() + "'");
        query.next();
        if(query.value(0).toString() == iterDB.key()) {

        }else{
            query.exec("INSERT INTO organisations (name) VALUES ('" + iterDB.key() + "')");
        }
    }

    baseDB.close(); //Закрытие соединения

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    ui->pushButton_2->setText("Удалить текущий \n лицевой счет");
    ui->pushButton_3->setText("Добавить текущий \n лицевой счет");
    ui->pushButton_4->setText("Добавить все лицевые счета");

    // Заполнение заголовков в таблице импорта
    ui->tableWidget->setColumnCount(6);
    QStandardItemModel *modelAccount = new QStandardItemModel();

    modelAccount->setHeaderData(0,Qt::Horizontal, "Наименование");
    modelAccount->setHeaderData(1,Qt::Horizontal, "Ном. номер");
    modelAccount->setHeaderData(2,Qt::Horizontal, "Ед. изм.");
    modelAccount->setHeaderData(3,Qt::Horizontal, "Категория");
    modelAccount->setHeaderData(4,Qt::Horizontal, "Количество");
    modelAccount->setHeaderData(5,Qt::Horizontal, "Лицевой счет");

    ui->tableAccount->setModel(modelAccount);
    ui->tableAccount->resizeColumnsToContents();
    ui->tableAccount->show();

    QStringList headers;
    headers.append("Наименование");
    headers.append("Ном. номер");
    headers.append("Ед. изм.");
    headers.append("Категория");
    headers.append("Количество");
    headers.append("Лицевой счет");
    ui->tableWidget->setHorizontalHeaderLabels(headers);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

    //ui->tableAccount->setHorizontalHeaderLabels(headers);
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
    currDB = baseDB;
    importBD(); //Импортирование из БД

    currDB.close();
    //Подключение первой вонской части (бд)
    currDB = DataBases[ui->selectVCh->itemText(0)];
    importBD();
    importOrgsAndHumans();
    docsFromBD();
    currDB.close();
}

//Функция открытия контексного меню в реестре нарядов
void MainWindow::showContextMenuOrders(const QPoint &pos) {
    QMenu menu;

    QAction *action0 = new QAction(QString::fromUtf8("Открыть распоряжение"), this);
    connect(action0, SIGNAL( triggered() ), this, SLOT(openOrderDetails()));

    QAction *action1 = new QAction(QString::fromUtf8("Загрузить скан"), this);
    connect(action1, SIGNAL( triggered() ), this, SLOT(downloadBtnAction()));

    QAction *action2 = new QAction(QString::fromUtf8("Открыть скан"), this);
    connect(action2, SIGNAL( triggered() ), this, SLOT(openBtnAction()));

    QAction *action3 = new QAction(QString::fromUtf8("Удалить распоряжение"), this);
    connect(action3, SIGNAL( triggered() ), this, SLOT(on_deleteOrder_clicked()));

    QAction *action4 = new QAction(QString::fromUtf8("Редактировать распоряжение"), this);
    connect(action4, SIGNAL( triggered() ), this, SLOT(on_redactOrder_clicked()));

    menu.addAction(action0);
    menu.addSeparator();
    menu.addAction(action1);
    //menu.addSeparator();
    menu.addAction(action2);
    menu.addSeparator();
    menu.addAction(action3);
    //menu.addSeparator();
    menu.addAction(action4);

    menu.exec(QCursor::pos());
}

//Функция открытия окна подробных данных о наряде
void MainWindow:: openOrderDetails() {

    order tempOrd;
    for(int i = 0; i < orders.size(); ++i) {
        if(orders[i].numberOutput == ui->tableWidgetOrders->item(ui->tableWidgetOrders->currentRow(), 2)->text()) {
            tempOrd = orders[i];
        }
    }

    OrderDetails *openOrdDetails = new OrderDetails(tempOrd, this);
    openOrdDetails->setModal(true);
    connect(openOrdDetails, SIGNAL(dataToMainWindow(int, product)), SLOT(dataFromAddItem(int, product)));
    openOrdDetails->exec();
}

//Функция импорта из БД
void MainWindow::importBD()
{
    //Заполнение контейнера АТИ данными из БД
    dataATI.clear();
    if(!baseDB.open()) {
        qDebug() << baseDB.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    QString queryText = "SELECT * FROM lic_scheta";
    QSqlQuery query(currDB), queryProduct(currDB);
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
            temp.releaseDate = queryProduct.value(8).toString();
            temp.factoryNumber = queryProduct.value(6).toString();

            currentSchet.push_back(temp);
        }

        dataATI.insert(schet, currentSchet);
    }

    baseDB.close();

    //Заполнение комбобокса лицевыми счетами из временного контейнера QMap
    QMapIterator <QString, QVector <product>> it(dataATI);
    ui->comboBoxScheta->clear();
    ui->comboBoxScheta->addItem("Все лицевые счета");
    while(it.hasNext()) {//Ошибка здесьььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььььь
        it.next();
        ui->comboBoxScheta->addItem(it.key());
    }

    on_comboBoxScheta_activated(ui->comboBoxScheta->itemText(0));
    ordersFromBD();
}

//Забираем данные о нарядах из БД
void MainWindow::ordersFromBD() {
    //Забираем данные о нарядах из БД

    if(!baseDB.open()) {
        qDebug() << baseDB.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    QSqlQuery queryOrders(baseDB);
    QString textQuery = "SELECT * FROM orders";
    queryOrders.exec(textQuery);
    qDebug() << queryOrders.lastError();

    while(queryOrders.next())
    {
        order tmp;
        tmp.name = queryOrders.value(1).toString();
        tmp.numberOutput = queryOrders.value(2).toString();
        bool flag = false;
        for(int i = 0; i < orders.size(); ++i) {
            if(orders[i].numberOutput == tmp.numberOutput) {
                flag = true;
            }
        }
        if(flag) continue;
        tmp.dateOutput = queryOrders.value(3).toString();
        tmp.numberInput = queryOrders.value(4).toString();

        tmp.dateInput = queryOrders.value(5).toString();
        tmp.chief = queryOrders.value(6).toString();
        tmp.oficier = queryOrders.value(7).toString();
        tmp.sendType = queryOrders.value(8).toString();
        tmp.base = queryOrders.value(9).toString();
        tmp.expirationDate = queryOrders.value(10).toString();
        tmp.tlg = queryOrders.value(11).toString();
        tmp.typeProduct = queryOrders.value(12).toString();
        tmp.completion = queryOrders.value(13).toInt();
        tmp.target = queryOrders.value(14).toString();

        //Забираем отправителей наряда из БД
        QString id_orders = queryOrders.value(0).toString();
        QSqlQuery query2(baseDB);
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
            tmpProd.measure = query2.value(3).toString();
            tmpProd.count = query2.value(4).toDouble();
            tmpProd.factoryNumber = query2.value(5).toString();
            tmpProd.category = query2.value(6).toInt();
            tmpProd.releaseDate = query2.value(7).toString();
            tmp.items.push_back(tmpProd);
        }
        //orders.clear();
        orders.push_back(tmp);
    }

    baseDB.close();

    //Добавляем в таблицу данные о нарядах
    //for(int i = 0; i < ui->tableWidgetOrders->rowCount(); ++i)
    //ui->tableWidgetOrders->removeRow(i);]
    int sz = orders.size();
    ui->tableWidgetOrders->setRowCount(orders.size());
    ui->tableWidgetOrders->setSortingEnabled(false);
    for(int i = 0; i < orders.size(); ++i)
    {
        QTableWidgetItem *item = new QTableWidgetItem();
        QTableWidgetItem *item1 = new QTableWidgetItem();
        QTableWidgetItem *item2 = new QTableWidgetItem();
        QTableWidgetItem *item3 = new QTableWidgetItem();
        QTableWidgetItem *item4 = new QTableWidgetItem();
        QTableWidgetItem *item5 = new QTableWidgetItem();
        QTableWidgetItem *item6 = new QTableWidgetItem();
        QTableWidgetItem *item7 = new QTableWidgetItem();
        QTableWidgetItem *item8 = new QTableWidgetItem();
        QTableWidgetItem *item9 = new QTableWidgetItem();

        item->setData(Qt::DisplayRole, i + 1);
        ui->tableWidgetOrders->setItem(i, 0, item);

        item1->setData(Qt::DisplayRole, orders[i].name.left(5));
        ui->tableWidgetOrders->setItem(i, 1, item1);

        item2->setData(Qt::DisplayRole, orders[i].numberOutput);
        ui->tableWidgetOrders->setItem(i, 2, item2);


        item3->setData(Qt::DisplayRole, QDate::fromString(orders[i].dateOutput,"dd.MM.yyyy"));
        ui->tableWidgetOrders->setItem(i, 3, item3);

        item4->setData(Qt::DisplayRole, orders[i].target);
        ui->tableWidgetOrders->setItem(i, 4, item4);

        item5->setData(Qt::DisplayRole, QDate::fromString(orders[i].expirationDate,"dd.MM.yyyy"));
        ui->tableWidgetOrders->setItem(i, 5, item5);

        item6->setData(Qt::DisplayRole, orders[i].senders[0]);
        ui->tableWidgetOrders->setItem(i, 6, item6);

        item7->setData(Qt::DisplayRole, orders[i].recipients[0]);
        ui->tableWidgetOrders->setItem(i, 7, item7);

        item8->setData(Qt::DisplayRole, orders[i].base);
        ui->tableWidgetOrders->setItem(i, 8, item8);

        item9->setData(Qt::DisplayRole, orders[i].completion);
        ui->tableWidgetOrders->setItem(i, 9, item9);
    }
    ui->tableWidgetOrders->setSortingEnabled(true);
    //ui->tableWidgetOrders->sortItems(3,Qt::AscendingOrder);
    //ui->tableWidgetOrders->horizontalHeader()
    //Выравнивание ячеек реестра по центру
    for (int i = 0; i < ui->tableWidgetOrders->rowCount(); ++i)
    {
        ui->tableWidgetOrders->setRowHeight(i, 32);
        for (int j = 0; j < ui->tableWidgetOrders->columnCount() - 4; ++j)
        {
            if (j != 8)
            ui->tableWidgetOrders->item(i, j)->setTextAlignment(Qt::AlignCenter);
        }
    }

    //Проверка распоряжений на просроченность
    for(int i = 0; i < ui->tableWidgetOrders->rowCount(); ++i){
        QVariant orderDate = ui->tableWidgetOrders->item(i, 5)->data(Qt::DisplayRole);
        QDate order = orderDate.toDate(), current = QDate::currentDate();
        if(order < current) {
            ui->tableWidgetOrders->item(i, 9)->setText("Просрочен");
            ui->tableWidgetOrders->item(i, 9)->setTextColor(Qt::red);
        }
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
    while (ui->tabWidget->count() > 1)
    {
        on_tabWidget_tabCloseRequested(1);
    }

    //ui->tabWidgetOrders->removeTab(1); // Удаления вкладки Зарегистировать распоряжения
}

void MainWindow::on_log_out_triggered()
{
    Entry *ent = new Entry;
    QObject::connect(ent, SIGNAL(close_signal(QString user, QString rights)), this, SLOT(close_entry(QString user, QString rights)));
    ent->setModal(true);
    ent->exec();
}
//Закрытие окна входа
void MainWindow::close_entry(QString user, QString rights) {
    currUser = user;
    currUserRights = rights;
    this->showMaximized();
    //delete ui;
}
//Удаление вкладки
void MainWindow::on_tabWidget_tabCloseRequested(int index)
{
    if(index != 0) {
        hidden_tabs->insert(ui->tabWidget->tabText(index), ui->tabWidget->widget(index));
        ui->tabWidget->removeTab(index);
    }
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
    for(int i = 0; i < ui->tabWidget->count(); ++i)
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
    if (check_available_tab("Наличие"))
    {
        new_tab("Наличие");
        on_selectVCh_activated("Все в/ч");
    }
}

void MainWindow::on_action_remont_triggered()
{
    if (check_available_tab("В ремонте"))
    {
        new_tab("В ремонте");
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
        on_pushButtonCancelMakeOrder_clicked(); //Стереть все данные из инпутов
        //on_pushButtonCancelOrder_3_clicked();
        ui->tabWidgetProvodPrihod_3->setCurrentIndex(0); //Фокус на вкладке основной информации при формировании нарядов
        ui->dateEditEndOrder->setDate(QDate::currentDate()); //Установка текущей даты в реестре нарядов

        ui->comboBoxTypeDocPrihod_3->lineEdit()->setPlaceholderText("Выберите тип документа");
        ui->comboBoxTypeDocPrihod_3->setCurrentIndex(-1);

        ui->orderTarget->lineEdit()->setPlaceholderText("Выберите цель операции");
        ui->orderTarget->setCurrentIndex(-1);

        //ui->orderTargetVDO->lineEdit()->setPlaceholderText("Выберите цель операции");
        //ui->orderTargetVDO->setCurrentIndex(-1);

        ui->comboBoxOrderSender->lineEdit()->setPlaceholderText("Выберите отправителя");
        ui->comboBoxOrderSender->setCurrentIndex(-1);

        ui->comboBoxOrderRecipient->lineEdit()->setPlaceholderText("Выберите получателя");
        ui->comboBoxOrderRecipient->setCurrentIndex(-1);

        ui->comboBoxTypeMoveOrder->lineEdit()->setPlaceholderText("Выберите порядок отправки");
        ui->comboBoxTypeMoveOrder->setCurrentIndex(-1);

        //Проверка сроков нарядов
        for(int i = 0; i < ui->tableWidgetOrders->rowCount(); ++i){
            QVariant orderDate = ui->tableWidgetOrders->item(i, 5)->data(Qt::DisplayRole);
            QDate order = orderDate.toDate(), current = QDate::currentDate();
            if(order < current) {
                ui->tableWidgetOrders->item(i, 9)->setText("Просрочен");
                ui->tableWidgetOrders->item(i, 9)->setTextColor(Qt::red);
            }
        }

        if(!baseDB.open()) {
            qDebug() << baseDB.lastError().text();
        }else{
            qDebug() << "It's OK!";
        }

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
        baseDB.close();
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

//Открытие вкладки "Подключение БД"
void MainWindow::on_connectBD_triggered()
{
    if (check_available_tab("Подключенные БД"))
    {
        new_tab("Подключенные БД");
    }
}

//Информация о текущей организации
void MainWindow::on_infoAboutOrg_triggered()
{

}
/////////////////////////////////////////////////////////////////

//Изменение критериев поиска
/*void MainWindow::on_comboBoxFindtypeAti_activated(int index)
{
    if(index == 0) ui->labelSprAti->setText("Введите номенклатурный номер:");
    if(index == 1) ui->labelSprAti->setText("Введите наименование:");
}*/

void MainWindow::on_comboBoxfindTypePptn_activated(int index)
{
    /*if(index == 0) ui->labelSprPptn->setText("Введите номенклатурный номер:");
    if(index == 1) ui->labelSprPptn->setText("Введите наименование:");*/
}

//Поиск в справочнике АТИ по н/н и наименованию
/*void MainWindow::on_findInAtiButton_clicked()
{
    QSqlQuery query (baseDB);
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
    ui->spr_ati_tableView->show();*/
//}

// Отображение выбранной группы АТИ
/*void MainWindow::on_findGroup_inAtiButton_clicked()
{
    QString text = "SELECT * FROM spr_ati WHERE nomkl_nom like \'17" + ui->comboBoxGroup_spr_ati->currentText().left(3) + "_____\'";
    QSqlQuery query(currDB);
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
}*/

//Поиск в справочнике ППТН
void MainWindow::on_findInPptnButton_clicked()
{

    if(!baseDB.open()) {
        qDebug() << baseDB.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    QSqlQuery query(baseDB);
    QString text;
    QSqlQueryModel *modelFind = new QSqlQueryModel;
    // Поиск по н/н
    /*if(ui->comboBoxfindTypePptn->currentIndex() == 0) {
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
    }*/
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
    baseDB.close();
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

        int i = 2;
        scheta.clear();
        QVector <product> prodsScheta;
        QString schet;
        while(import.read(i, 1).toString() != "")
        {
           if(import.read(i, 3).toString() == "" && import.read(i, 2).toString() == "" && import.read(i, 4).toString() == "") {
                schet = import.read(i, 1).toString();
                i++;
           }
           //Заполнение вектора имуществом
           product temp;
           temp.nomNumber = import.read(i, 2).toString();
           temp.name = import.read(i, 3).toString();
           temp.measure = import.read(i, 4).toString();
           temp.category = import.read(i, 5).toInt();
           temp.count = import.read(i, 6).toDouble();
           prodsScheta.push_back(temp);
           i++;
           if(import.read(i, 3).toString() == "")
           {
                scheta.insert(schet, prodsScheta);
                prodsScheta.clear();
           }
        }
        //Заполнение комбобокса лицевыми счетами из временного контейнера QMap
        ui->comboBox_2->clear();
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
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(currentSchet[i].category)));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(currentSchet[i].count)));
        ui->tableWidget->setItem(i, 5, new QTableWidgetItem(QString::number(currentSchet[i].price,'g', 10)));
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
        QSqlQuery query(currDB);
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
                queryText = "INSERT INTO " + typeProduct + " (name, nom_numb, price, measure, count, category, id_lic_scheta) VALUES (\"" + currentSchet[i].name + "\", \"" + currentSchet[i].nomNumber + "\", \"" + QString::number(currentSchet[i].price, 'g', 10) + "\", \"" +
                                           currentSchet[i].measure + "\", \"" + QString::number(currentSchet[i].count, 'g', 10) + "\", \"" +  QString::number(currentSchet[i].category) + "\", \"" + QString::number(id) + "\")";
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

    //Удаление лицевого счета из временного хранилища и комбобокса
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
        add_lic_schet(ui->comboBox_2->currentText());
        ui->comboBox_2->removeItem(ui->comboBox_2->currentIndex());
    }
    ui->statusBar->showMessage("Все лицевые счета из импортированного файла Excel добавлены в базу данных.");
}

//Отображение имущества на выбранном лицевом счете
void MainWindow::on_comboBoxScheta_activated(const QString &arg1)
{
    QVector <product> currentSchet;

    QStandardItemModel *modelAccount = new QStandardItemModel();

        //Выбор типа имущества
    if(arg1 != "Все лицевые счета") {
        if(ui->comboBoxTypeProduct->currentText() == "АТИ")
        {
            currentSchet = dataATI[arg1];
        }else{
            currentSchet = dataPPTN[arg1];
        }

        int len = currentSchet.size();

        //ui->tableAccount->setRowCount(len);

        QStringList wordListNames;


        for (int i = 0; i < len; ++i) {
            /*ui->tableAccount->setItem(i, 0, new QTableWidgetItem(currentSchet[i].name));
            ui->tableAccount->setItem(i, 1, new QTableWidgetItem(currentSchet[i].nomNumber));
            ui->tableAccount->setItem(i, 2, new QTableWidgetItem(currentSchet[i].measure));
            ui->tableAccount->setItem(i, 3, new QTableWidgetItem(QString::number(currentSchet[i].category)));
            ui->tableAccount->setItem(i, 4, new QTableWidgetItem(QString::number(currentSchet[i].count)));
            ui->tableAccount->setItem(i, 5, new QTableWidgetItem(arg1));*/

            QList<QStandardItem*> lst;
            QStandardItem *item = new QStandardItem(currentSchet[i].name);
            QStandardItem *item1 = new QStandardItem(currentSchet[i].nomNumber);
            QStandardItem *item2 = new QStandardItem(currentSchet[i].measure);
            QStandardItem *item3 = new QStandardItem(QString::number(currentSchet[i].category));
            QStandardItem *item4 = new QStandardItem(QString::number(currentSchet[i].count));
            QStandardItem *item5 = new QStandardItem(arg1);

            lst <<item<<item1<<item2<<item3<<item4<<item5;
            modelAccount->appendRow(lst);
            wordListNames.append(currentSchet[i].name);
        }

        modelAccount->setHeaderData(0,Qt::Horizontal, "Наименование");
        modelAccount->setHeaderData(1,Qt::Horizontal, "Ном. номер");
        modelAccount->setHeaderData(2,Qt::Horizontal, "Ед. изм.");
        modelAccount->setHeaderData(3,Qt::Horizontal, "Категория");
        modelAccount->setHeaderData(4,Qt::Horizontal, "Количество");
        modelAccount->setHeaderData(5,Qt::Horizontal, "Лицевой счет");

        ui->tableAccount->setModel(modelAccount);
        ui->tableAccount->resizeColumnsToContents();
        ui->tableAccount->horizontalHeader()->setStretchLastSection(true);
        ui->tableAccount->show();

        //Поиск с подсказками
        QCompleter *completer = new QCompleter(wordListNames, this);
        completer->setCaseSensitivity(Qt::CaseInsensitive);
        ui->lineEditFindInSklad->setCompleter(completer);
    }else{
        for(int i = 1, len = 0; i < ui->comboBoxScheta->count(); ++i) {
            QString schet = ui->comboBoxScheta->itemText(i);
            currentSchet = dataATI[schet];
            int bgn = len;
            len += currentSchet.size();

            //ui->tableAccount->setRowCount(len);

            QStringList wordListNames;

            for (int j = 0; j < currentSchet.size(); ++j) {
                /*ui->tableAccount->setItem(bgn, 0, new QTableWidgetItem(currentSchet[j].name));
                ui->tableAccount->setItem(bgn, 1, new QTableWidgetItem(currentSchet[j].nomNumber));
                ui->tableAccount->setItem(bgn, 2, new QTableWidgetItem(currentSchet[j].measure));
                ui->tableAccount->setItem(bgn, 3, new QTableWidgetItem(QString::number(currentSchet[j].category)));
                ui->tableAccount->setItem(bgn, 4, new QTableWidgetItem(QString::number(currentSchet[j].count)));
                ui->tableAccount->setItem(bgn, 5, new QTableWidgetItem(schet));*/

                QList<QStandardItem*> lst;
                QStandardItem *item = new QStandardItem(currentSchet[j].name);
                QStandardItem *item1 = new QStandardItem(currentSchet[j].nomNumber);
                QStandardItem *item2 = new QStandardItem(currentSchet[j].measure);
                QStandardItem *item3 = new QStandardItem(QString::number(currentSchet[j].category));
                QStandardItem *item4 = new QStandardItem(QString::number(currentSchet[j].count));
                QStandardItem *item5 = new QStandardItem(schet);

                lst <<item<<item1<<item2<<item3<<item4<<item5;
                modelAccount->appendRow(lst);

                wordListNames.append(currentSchet[j].name);
                //++bgn;
            }


            //Поиск с подсказками
            QCompleter *completer = new QCompleter(wordListNames, this);
            completer->setCaseSensitivity(Qt::CaseInsensitive);
            ui->lineEditFindInSklad->setCompleter(completer);
        }

        modelAccount->setHeaderData(0,Qt::Horizontal, "Наименование");
        modelAccount->setHeaderData(1,Qt::Horizontal, "Ном. номер");
        modelAccount->setHeaderData(2,Qt::Horizontal, "Ед. изм.");
        modelAccount->setHeaderData(3,Qt::Horizontal, "Категория");
        modelAccount->setHeaderData(4,Qt::Horizontal, "Количество");
        modelAccount->setHeaderData(5,Qt::Horizontal, "Лицевой счет");

        ui->tableAccount->setModel(modelAccount);
       /*for(int i = 1, len = 0; i < ui->comboBoxScheta->count(); ++i) {
            QString schet = ui->comboBoxScheta->itemText(i);
            int cnt = dataATI[schet].size();
            ui->tableAccount->setSpan(len, 5, len + cnt, 5);
            len += cnt;
        }*/

        ui->tableAccount->resizeColumnsToContents();
        ui->tableAccount->horizontalHeader()->setStretchLastSection(true);
        ui->tableAccount->resizeColumnsToContents();
        ui->tableAccount->show();
    }

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
        QDate temp(2020, 1, 1);
        dateSort(temp, QDate::currentDate());
    }else{
        ui->dateEditBeginOrder->setEnabled(true);
        ui->dateEditEndOrder->setEnabled(true);
        dateSort(ui->dateEditBeginOrder->date(), ui->dateEditEndOrder->date());
    }
}
void MainWindow::on_pushButtonNextPrihod_2_clicked()
{
    order temp;
    temp.name = ui->comboBoxTypeOrder->currentText();
    temp.base = ui->lineEditBaseOrder->text();
    temp.numberOutput = ui->lineEditOutputOrder->text();
    temp.numberInput = ui->lineEditInputOrder->text();
    temp.dateOutput = ui->dateOutputOrder->text();
    temp.dateInput = ui->dateInputOrder->text();
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
            ui->comboboxEditDepart->setCurrentIndex(-1);
            ui->comboboxEditDepart->setToolTip("Введите наименование нового подразделения!");
            ui->comboboxEditDepart->setFocus();
        }else{
            ui->comboboxEditDepart->addItem(temp);
            QVector <human> tempHumans;
            tempDeps.insert(temp, tempHumans); //Добавление подразделения и пустого списка сотрудников во временное хранилище
            ui->statusBar->showMessage("Подразделение '" + temp + "' добавлено.");

            //Удаление с экрана данных сотрудника
            ui->addWorkerPosition->clear();
            ui->addWorkerRank->clear();
            ui->addWorkerSecName->clear();
            ui->addWorkerName->clear();
            ui->addWorkerMidName->clear();
            ui->addWorkerPhonenumber->clear();
            ui->addWorkerEmail->clear();
            ui->comboboxEditWorker->clear();
            ui->comboboxEditWorker->addItem("Добавьте нового сотрудника");

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
        ui->statusBar->showMessage("Подразделение '" + currentDep + "' удалено.");
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

    //Добавление сотрудника в КомбоБокс
    if(ui->comboboxEditWorker->currentIndex() == -1)
    {
        tempDeps[ui->comboboxEditDepart->currentText()].push_back(tempHuman);
        ui->comboboxEditWorker->addItem(tempHuman.position + " - " + tempHuman.rank + " " + tempHuman.FIO);
        ui->statusBar->showMessage("Сотрудник \"" + tempHuman.FIO + "\" добавлен.");
    }else{
        tempDeps[ui->comboboxEditDepart->currentText()][ui->comboboxEditWorker->currentIndex()] = tempHuman;
        ui->comboboxEditWorker->setCurrentText(tempHuman.position + " - " + tempHuman.rank + " " + tempHuman.FIO);
        ui->statusBar->showMessage("Данные сотрудника \"" + tempHuman.FIO + "\" изменены.");
    }
}

//Загрузка сотрудников при выборе подразделения (изменение организаций)
void MainWindow::on_comboboxEditDepart_activated(const QString &arg1)
{
    ui->comboboxEditWorker->clear();
    organisation tempOrg = organisationFromDB(ui->addNameOrg->text());
    for(int i = 0; i < tempOrg.departList.size(); ++i) {
        if(tempOrg.departList[i].name == arg1) {
            for(int j = 0; j < tempOrg.departList[i].humanList.size(); ++j) {
                ui->comboboxEditWorker->addItem(tempOrg.departList[i].humanList[j].FIO);
            }
        }
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

    if(!baseDB.open()) {
        qDebug() << baseDB.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    QSqlQuery query(baseDB);

    query.exec("SELECT name FROM organisations WHERE name = '" + tempOrg.name + "'");
    qDebug() << query.lastError();
    query.next();
    bool redFlag = false;
    if(query.value(0).toString() == tempOrg.name) {
        redFlag = true;
    }

    if(redFlag) {
        query.exec("UPDATE organisations SET inn = '" + tempOrg.inn + "', phoneNumber = '" + tempOrg.phoneNumber + "', city = '" + tempOrg.city +
                   "', postCode = '" + tempOrg.index + "', vpMORF = '" + tempOrg.vpMORF + "' WHERE name = '" + tempOrg.name + "'");
    qDebug() << query.lastError();
    }else{
        QString queryText = "INSERT INTO organisations (name, inn, phoneNumber, city, postCode, vpMORF) VALUES ('"
                + tempOrg.name + "', '" + tempOrg.inn + "', '" + tempOrg.phoneNumber + "', '" + tempOrg.city + "', '" + tempOrg.index + "', '" + tempOrg.vpMORF + "')";
        query.exec(queryText);
        qDebug() << query.lastError();
    }

    QString queryText = "SELECT id FROM organisations WHERE name = '" + tempOrg.name + "'";
    query.exec(queryText);
    qDebug() << query.lastError();

    query.next();  // Добавить отлов возможных ошибок
    int idOrg = query.value(0).toInt();

    queryText = "SELECT id FROM departments_of_org WHERE id_organisation = " + QString::number(idOrg);
    query.exec(queryText);
    qDebug() << query.lastError();
    //Удаление сотрудников
    while(query.next()) {
        QSqlQuery delQuery(baseDB);
        delQuery.exec("DELETE FROM humans WHERE id_department = " + query.value(0).toString());
        qDebug() << query.lastError();
    }
    //Удаление отделов
    query.exec("DELETE FROM departments_of_org WHERE id_organisation = " + QString::number(idOrg));
    qDebug() << query.lastError();

    QMapIterator <QString, QVector <human> > it(tempDeps);
    while(it.hasNext())
    {
        it.next();
        QString depName = it.key();
        queryText = "INSERT INTO departments_of_org (name, id_organisation) VALUES ('"
                + depName + "', " + QString::number(idOrg) + ")";
        query.exec(queryText);
        qDebug() << query.lastError();

        queryText = "SELECT id FROM departments_of_org WHERE name = '" + depName + "' AND id_organisation = " + QString::number(idOrg);
        query.exec(queryText);
        qDebug() << query.lastError();

        query.next();  // Добавить отлов возможных ошибок
        int idDep = query.value(0).toInt();

        for(int i = 0; i < it.value().size(); ++i)
        {
            queryText = "INSERT INTO humans (position, FIO, firstName, secondName, middleName, rank, phoneNumber, email, id_department) VALUES ('" +
                    it.value()[i].position + "', '" + it.value()[i].FIO + "', '" + it.value()[i].firstName + "', '" + it.value()[i].secondName + "', '" +
                    it.value()[i].middleName + "', '" + it.value()[i].rank + "', '" + it.value()[i].phoneNumber + "', '" + it.value()[i].email + "', " +
                    QString::number(idDep) + ")";
            query.exec(queryText);
        }
    }

    baseDB.close();
    tempDeps.clear();

    importOrgsAndHumans();
    //Переключение фокуса на вкладку с организациями
    on_buttonCancelAddOrg_clicked(); //Очистка вкладок
    ui->tabWidgetOrganisations->setTabText(2, "Добавить организацию");
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

    ui->comboboxEditDepart->clear();
    ui->comboboxEditWorker->clear();

    //Удаление с экрана данных об организации
    ui->addNameOrg->clear();
    ui->addCityOrg->clear();
    ui->addPostcodeOrg->clear();
    ui->addInnOrg->clear();
    ui->addPhonenumberOrg->clear();
    ui->addVpMORFOrg->clear();

    //Очищение контейнера с организациями
    tempDeps.clear();

    //Переключение фокуса на вкладку с организациями
    ui->tabWidgetOrganisations->setTabText(2, "Добавить организацию");
    ui->tabWidgetOrganisations->setCurrentIndex(0);
}

//Импорт организаций и сотрудников
void MainWindow::importOrgsAndHumans()
{
    if(!baseDB.open()) {
        qDebug() << baseDB.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    QSqlQuery query(baseDB), query2(baseDB), query3(baseDB);
    QString textQuery = "SELECT * FROM organisations";
    query.exec(textQuery);
    qDebug() << query.lastError();

    ui->tableWidgetOrgs->setRowCount(0); //Сброс таблицы с организациями
    ui->comboBoxOrderRecipient->clear();
    ui->comboBoxOrderRecipient->addItem("Выберите получателя...");
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

        ui->tableWidgetOrgs->setRowCount(ui->tableWidgetOrgs->rowCount() + 1);
        int i = ui->tableWidgetOrgs->rowCount() - 1;
        ui->tableWidgetOrgs->setItem(i, 0, new QTableWidgetItem(tempOrg.name));
        ui->tableWidgetOrgs->setItem(i, 1, new QTableWidgetItem(tempOrg.city));

        if(ui->comboBoxOrderRecipient->findText(tempOrg.name) == -1)
        ui->comboBoxOrderRecipient->addItem(tempOrg.name);

        if(ui->comboBoxRecipientOrder->findText(tempOrg.name) == -1)
        ui->comboBoxRecipientOrder->addItem(tempOrg.name);
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
    baseDB.close();

}

//Загрузка сотрудников при выборе подразделения (просмотр организаций)
void MainWindow::on_comboboxSelectDepart_activated(const QString &arg1)
{
    ui->comboboxSelectWorker->clear();
    organisation tempOrg = organisationFromDB(ui->nameOrg->text());
    for(int i = 0; i < tempOrg.departList.size(); ++i) {
        if(tempOrg.departList[i].name == arg1) {
            for(int j = 0; j < tempOrg.departList[i].humanList.size(); ++j) {
                ui->comboboxSelectWorker->addItem(tempOrg.departList[i].humanList[j].FIO);
            }
        }
    }
    ui->comboboxSelectWorker->setCurrentIndex(-1);
}

// Загрузка данных о сотруднике при выборе его из списка (просмотр организаций)-----------------------------------Удалить функцию
void MainWindow::on_comboboxSelectWorker_activated(int index)
{

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
    on_pushButtonCancelOderMC_clicked();
}

//Нажатие кнопки "Отмена" на вкладке регистрации распоряжения (Перечень МЦ)
void MainWindow::on_pushButtonCancelOderMC_clicked()
{
    ui->comboBoxTypeOrder->setCurrentIndex(0);
    ui->lineEditOutputOrder->clear();
    ui->dateOutputOrder->setDate(QDate(2000, 1, 1));
    ui->lineEditInputOrder->clear();
    ui->dateInputOrder->setDate(QDate(2000, 1, 1));
    ui->dateEditExpiration->setDate(QDate(2000, 1, 1));
    ui->lineEditBaseOrder->clear();
    ui->comboBoxSenderOrder->setCurrentIndex(0);
    ui->comboBoxRecipientOrder->setCurrentIndex(0);
    ui->tableWidgetOrderVDO->clearContents();
    ui->tableWidgetOrderVDO->setRowCount(0);

    ui->tabWidgetRegOrder->setCurrentIndex(0);
    ui->tabWidgetOrders->setCurrentIndex(0);

    ui->pushButtonDoneMakeOrder->setText("Сформировать");
    ui->pushButtonDoneOrder->setText("Зарегистрировать");
    redactFlag = false;
}

//Функция добавления Распоряжения в БД
void MainWindow::addOrderToDB(order tempOrder)
{
    if(!baseDB.open()) {
        qDebug() << baseDB.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    QSqlQuery query (baseDB);
    QString textQuery = "INSERT INTO orders (name, numberOutput, dateOutput, numberInput, dateInput, base, expirationDate, completion, sendType, target, typeProduct) VALUES ('" +
            tempOrder.name + "', '" + tempOrder.numberOutput + "', '" + tempOrder.dateOutput + "', '" + tempOrder.numberInput + "', '" +
            tempOrder.dateInput + "', '" + tempOrder.base + "', '" + tempOrder.expirationDate + "', " + QString::number(tempOrder.completion) + ", '" + tempOrder.sendType + "', '" + tempOrder.target + "', '" + tempOrder.typeProduct + "')";
    query.exec(textQuery);
    qDebug() << query.lastError();

    //Забираем из БД id распоряжения
    query.exec("SELECT id FROM orders WHERE numberOutput = '" + tempOrder.numberOutput + "' AND dateOutput = '" + tempOrder.dateOutput + "'");
    qDebug() << query.lastError();
    query.next();
    qDebug() << query.lastError();
    QString id_order = query.value(0).toString();
    //Добавление Отправителей в наряд
    for(int i = 0; i < tempOrder.senders.size(); ++i)
    {
        textQuery = "INSERT INTO orderSenders (sender, id_orders) VALUES ('" + tempOrder.senders[i] + "', " + id_order + ")";
        query.exec(textQuery);
        qDebug() << query.lastError();
    }

    //Добавление получателей в наряд
    for(int i = 0; i < tempOrder.recipients.size(); ++i)
    {
        textQuery = "INSERT INTO recipientsOrder (recipient, id_orders) VALUES ('" + tempOrder.recipients[i] + "', " + id_order + ")";
        query.exec(textQuery);
        qDebug() << query.lastError();
    }

    for(int i = 0; i < tempOrder.items.size(); ++i)
    {
        textQuery = "INSERT INTO product_orders (name, nom_numb, measure, count, factoryNumber, category, releaseDate, id_orders) VALUES ('" + tempOrder.items[i].name + "', '"
                + tempOrder.items[i].nomNumber + "', '" + tempOrder.items[i].measure + "', '" + QString::number(tempOrder.items[i].count, 'g', 2) + "', '"
                + tempOrder.items[i].factoryNumber + "', '" + QString::number(tempOrder.items[i].category) + "', '" + tempOrder.items[i].releaseDate + "', " + id_order + ")";
        query.exec(textQuery);
        qDebug() << query.lastError();
        qDebug() << textQuery;
    }
    //orders.push_back(tempOrder);
    //Обновление списка зарегистрированных нарядов
    ui->tableWidgetOrders->setRowCount(orders.size());
    for(int i = 0; i < orders.size(); ++i)
    {
        QTableWidgetItem *item = new QTableWidgetItem();
        QTableWidgetItem *item1 = new QTableWidgetItem();
        QTableWidgetItem *item2 = new QTableWidgetItem();
        QTableWidgetItem *item3 = new QTableWidgetItem();
        QTableWidgetItem *item4 = new QTableWidgetItem();
        QTableWidgetItem *item5 = new QTableWidgetItem();
        QTableWidgetItem *item6 = new QTableWidgetItem();
        QTableWidgetItem *item7 = new QTableWidgetItem();
        QTableWidgetItem *item8 = new QTableWidgetItem();
        QTableWidgetItem *item9 = new QTableWidgetItem();

        item->setData(Qt::DisplayRole, i + 1);
        ui->tableWidgetOrders->setItem(i, 0, item);

        item1->setData(Qt::DisplayRole, orders[i].name.left(5));
        ui->tableWidgetOrders->setItem(i, 1, item1);

        item2->setData(Qt::DisplayRole, orders[i].numberOutput);
        ui->tableWidgetOrders->setItem(i, 2, item2);


        item3->setData(Qt::DisplayRole, QDate::fromString(orders[i].dateOutput,"dd.MM.yyyy"));
        ui->tableWidgetOrders->setItem(i, 3, item3);

        item4->setData(Qt::DisplayRole, orders[i].target);
        ui->tableWidgetOrders->setItem(i, 4, item4);

        item5->setData(Qt::DisplayRole, QDate::fromString(orders[i].expirationDate,"dd.MM.yyyy"));
        ui->tableWidgetOrders->setItem(i, 5, item5);

        item6->setData(Qt::DisplayRole, orders[i].senders[0]);
        ui->tableWidgetOrders->setItem(i, 6, item6);

        item7->setData(Qt::DisplayRole, orders[i].recipients[0]);
        ui->tableWidgetOrders->setItem(i, 7, item7);

        item8->setData(Qt::DisplayRole, orders[i].base);
        ui->tableWidgetOrders->setItem(i, 8, item8);

        item9->setData(Qt::DisplayRole, orders[i].completion);
        ui->tableWidgetOrders->setItem(i, 9, item9);


    }

    ui->tabWidgetOrders->setCurrentIndex(1);
    baseDB.close();
}
//Нажатие кнопки "Зарегистрировать" на вкладке распоряжения (Перечень МЦ)
void MainWindow::on_pushButtonDoneOrder_clicked()
{
    if(redactFlag) {
        on_deleteOrder_clicked();
        ui->pushButtonDoneOrder->setText("Зарегистрировать");
        redactFlag = false;
    }
    QVector <product> tempProdContainer;

    for(int i = 0; i < ui->tableWidgetOrderVDO->rowCount(); ++i)
    {
        product tempProd;

        tempProd.name = ui->tableWidgetOrderVDO->item(i, 0)->text();
        if(ui->tableWidgetOrderVDO->item(i, 1)->text() != "")
        {
            tempProd.nomNumber = ui->tableWidgetOrderVDO->item(i, 1)->text();
        }else{
            tempProd.nomNumber = "б/н";
        }

        tempProd.factoryNumber = ui->tableWidgetOrderVDO->item(i, 2)->text();

        if(ui->tableWidgetOrderVDO->item(i, 3)->text() != "")
        {
            tempProd.releaseDate = ui->tableWidgetOrderVDO->item(i, 3)->text();
        }

        tempProd.category = ui->tableWidgetOrderVDO->item(i, 4)->text().toInt();
        tempProd.measure = ui->tableWidgetOrderVDO->item(i, 5)->text();
        tempProd.count =  ui->tableWidgetOrderVDO->item(i, 6)->text().toDouble();
        tempProd.orderNumber = tempOrder.name + " №" + tempOrder.numberOutput + " от " + tempOrder.dateOutput; // ????nm

        tempProdContainer.push_back(tempProd);
    }

    tempOrder.clear();

    tempOrder.name = ui->comboBoxTypeOrder->currentText();
    tempOrder.numberOutput = ui->lineEditOutputOrder->text();
    tempOrder.dateOutput = ui->dateOutputOrder->text();
    tempOrder.numberInput = ui->lineEditInputOrder->text();
    tempOrder.dateInput = ui->dateInputOrder->text();
    tempOrder.expirationDate = ui->dateEditExpiration->text();
    tempOrder.base = ui->lineEditBaseOrder->text();
    tempOrder.senders.push_back(ui->comboBoxSenderOrder->currentText());
    tempOrder.recipients.push_back(ui->comboBoxRecipientOrder->currentText());
    //tempOrder.scans = "ВДО";
    tempOrder.sendType = ui->comboBoxSendType->currentText();
    tempOrder.target = ui->orderTargetVDO->currentText().left(2);
    tempOrder.items = tempProdContainer;
    tempOrder.typeProduct = "ВДО";

    orders.push_back(tempOrder);

    //Добавление распоряжения в БД
    addOrderToDB(tempOrder);
    ordersFromBD();

    //Очистка вкладки регистрации
    on_pushButtonCancelOderMC_clicked();

}


//Нажатие кнопки "Обзор" для выбора пути до БД
void MainWindow::on_importExcelpushButton_2_clicked()
{
    QStringList dbPaths = QFileDialog::getOpenFileNames(this, "Выберите файл (файлы) БД ...", "","*.db");
    ui->lineEdit_2->setText(dbPaths.last());
    //connectedDB(dbPaths);

    /*QDir dir (QDir::home());
    dir.setPath(dir.path() + "//DataBases");
    QFileInfoList infoList = dir.entryInfoList(QStringList() << "*.db" , QDir::Files);
    QStringList dbPaths;

    for(int i = 0; i < infoList.size(); ++i) {
        dbPaths.push_back(infoList.at(i).absoluteFilePath());
    }*/

    ui->lineEdit_2->setText(dbPaths.last());
    connectedDB(dbPaths);

    QMapIterator <QString, QSqlDatabase> iterDB(DataBases);
    while(iterDB.hasNext())
    {
        iterDB.next();
        ui->comboBoxOrderSender->addItem(iterDB.key());
        //ui->comboBoxOrderRecipient->addItem(iterDB.key());
        ui->selectVCh->addItem(iterDB.key());
    }
    //Подключение первой вонской части (бд)
    currDB = DataBases[ui->selectVCh->itemText(0)];
    importBD();
    importOrgsAndHumans();
}

//Подключение выбранных БД
void MainWindow::connectedDB(QStringList dbPaths) {
    int currRows = ui->tableWidgetConnectedBD->rowCount();
    ui->tableWidgetConnectedBD->setRowCount(currRows + dbPaths.size());
    QVector <QString> numbersDB;
    for(int i = 0; i < 100; ++i)
    {
       numbersDB.push_back(QString::number(i));
    }
    if(dbPaths.size() > 0)
    {
        for(int i = 0; i < dbPaths.size(); ++i)
        {
            QSqlDatabase tempDb = QSqlDatabase::addDatabase("QSQLITE", numbersDB[i]);
            tempDb.setDatabaseName(dbPaths[i]);

            if(!tempDb.open()) {
                qDebug() << tempDb.lastError().text();
                ui->tableWidgetConnectedBD->setItem(currRows + i, 1, new QTableWidgetItem(tempDb.lastError().text()));
            }else{
                qDebug() << "It's OK!";
                ui->tableWidgetConnectedBD->setItem(currRows + i, 1, new QTableWidgetItem("Подключено"));
            }

            ui->tableWidgetConnectedBD->setItem(currRows + i, 2, new QTableWidgetItem(dbPaths[i]));
            QSqlQuery tempQuery(tempDb);
            QString text = "SELECT name FROM organisations WHERE checkMyOrg = 1";
            tempQuery.exec(text);
            tempQuery.next();
            qDebug() << tempQuery.lastError();
            qDebug() << tempQuery.value(0).toString();
            ui->tableWidgetConnectedBD->setItem(currRows + i, 0, new QTableWidgetItem(tempQuery.value(0).toString()));
            //ui->comboBoxSenderOrder->addItem(tempQuery.value(0).toString());
            //ui->comboBoxRecipientOrder->addItem(tempQuery.value(0).toString());
            DataBases.insert(tempQuery.value(0).toString(), tempDb);
            text = "INSERT INTO dataBases (dbName, dbPath) VALUES ('" + tempQuery.value(0).toString() + "', '" + dbPaths[i] + "')";
            tempQuery.exec(text);
            qDebug() << tempQuery.lastError();
        }
        ui->lineEdit->setText(dbPaths[0]);
    }
}

//Использование выбранной БД
void MainWindow::on_useBD_clicked()
{
    currDB = DataBases[ui->tableWidgetConnectedBD->item(ui->tableWidgetConnectedBD->currentRow(), 0)->text()];
    importBD();
    importOrgsAndHumans();
    ui->tableWidgetConnectedBD->item(ui->tableWidgetConnectedBD->currentRow(), 0)->setBackgroundColor(Qt::green);
    ui->tableWidgetConnectedBD->item(ui->tableWidgetConnectedBD->currentRow(), 1)->setBackgroundColor(Qt::green);
    ui->tableWidgetConnectedBD->item(ui->tableWidgetConnectedBD->currentRow(), 2)->setBackgroundColor(Qt::green);
    //ui->centralWidget->setWindowTitle("ATI.Poryadok - " /*+ ui->tableWidgetConnectedBD->item(ui->tableWidgetConnectedBD->currentRow(), 0)->text()*/);
}

//Активация кнопок при выборе БД
void MainWindow::on_tableWidgetConnectedBD_cellClicked(int row, int column)
{
    ui->deleteBD->setEnabled(true);
    ui->useBD->setEnabled(true);
    ui->changeBD->setEnabled(true);
}

void MainWindow::on_tableWidgetConnectedBD_cellActivated(int row, int column)
{

}

//Выбор отправителя в наряде
void MainWindow::on_comboBoxOrderSender_activated(const QString &arg1)
{
    ui->tableWidgetOrderSenderATI->setRowCount(0);
    ui->comboBoxShetaOfSender->clear();

    //ui->comboBoxShetaOfSender->addItem("Выберите лицевой счет отправителя");
    senderDB = DataBases[arg1];

    if(!senderDB.open()) {
        qDebug() << senderDB.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    QString queryText = "SELECT * FROM lic_scheta";
    QSqlQuery query(senderDB), queryProduct(senderDB);
    query.exec(queryText);
    qDebug() << query.lastError();
    QStringList wordList;
    while(query.next())
    {
        //Считывание лицевых счетов
        QString id = query.value(0).toString();
        QString schet = query.value(1).toString();
        ui->comboBoxShetaOfSender->addItem(schet); //Добавление лицевых счетов в КомбоБокс
        wordList.append(schet);

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
            temp.releaseDate = queryProduct.value(8).toString();
            temp.factoryNumber = queryProduct.value(6).toString();

            currentSchet.push_back(temp);
        }

        orderDataATI.insert(schet, currentSchet);
    }

    senderDB.close();
    QCompleter *completer = new QCompleter(wordList, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->comboBoxShetaOfSender->setCompleter(completer);
    ui->comboBoxShetaOfSender->lineEdit()->setPlaceholderText("Выберите лицевой счет отправителя");
    ui->comboBoxShetaOfSender->setCurrentIndex(-1);
}

//Выбор лицевого счета отправителя
void MainWindow::on_comboBoxShetaOfSender_activated(const QString &arg1)
{
    QVector <product> currentSchet;
    currentSchet = orderDataATI[arg1];

    int len = currentSchet.size();

    QStringList wordListNames;

    ui->tableWidgetOrderSenderATI->clear();
    ui->tableWidgetOrderSenderATI->setRowCount(len);

    ui->pushButtonFindPrihod_3->setEnabled(true);
    //QPushButton *addBtn = new QPushButton("Добавить");


    for (int i = 0; i < len; ++i)
    {
        QPushButton *addBtn = new QPushButton("Добавить");
        addBtn->setFixedSize(128, 30);

        ui->tableWidgetOrderSenderATI->setItem(i, 0, new QTableWidgetItem(currentSchet[i].name));
        ui->tableWidgetOrderSenderATI->setItem(i, 1, new QTableWidgetItem(currentSchet[i].nomNumber));
        ui->tableWidgetOrderSenderATI->setItem(i, 2, new QTableWidgetItem(currentSchet[i].measure));
        ui->tableWidgetOrderSenderATI->setItem(i, 3, new QTableWidgetItem(QString::number(currentSchet[i].category)));
        ui->tableWidgetOrderSenderATI->setItem(i, 4, new QTableWidgetItem(QString::number(currentSchet[i].count)));
        ui->tableWidgetOrderSenderATI->setItem(i, 5, new QTableWidgetItem("")); //Добавить дату выпуска
        ui->tableWidgetOrderSenderATI->setItem(i, 6, new QTableWidgetItem("")); //Добавить заводской номер

        ui->tableWidgetOrderSenderATI->setIndexWidget(ui->tableWidgetOrderSenderATI->model()->index(i, 8), addBtn);

        ui->tableWidgetOrderSenderATI->item(i, 1)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrderSenderATI->item(i, 2)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrderSenderATI->item(i, 3)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrderSenderATI->item(i, 4)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrderSenderATI->item(i, 5)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrderSenderATI->item(i, 6)->setTextAlignment(Qt::AlignCenter);
        //ui->tableWidgetOrderSenderATI->item(i, 7)->setTextAlignment(Qt::AlignCenter);

        connect(addBtn, SIGNAL( clicked( bool ) ), SLOT( addBtnAction() ) );
        wordListNames.append(currentSchet[i].name);
    }
    //ui->tableWidgetOrderSenderATI->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Fixed);


    QCompleter *completer = new QCompleter(wordListNames, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEditFindOrderItem->setCompleter(completer);

    QStringList headers;
    headers.append("Наименование");
    headers.append("Номенклатурный номер");
    headers.append("Ед. изм.");
    headers.append("Категория");
    headers.append("Количество");
    headers.append("Дата выпуска");
    headers.append("Заводской номер");
    headers.append("");
    headers.append("Действие");
    ui->tableWidgetOrderSenderATI->setHorizontalHeaderLabels(headers);

    ui->tableWidgetOrderSenderATI->resizeColumnsToContents();
    //ui->tableWidgetOrderSenderATI->horizontalHeader()->setStretchLastSection(true);
    //ui->tableWidgetOrderSenderATI->setColumnWidth(7, 128);
    ui->tableWidgetOrderSenderATI->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);

    ui->tableWidgetOrderSenderATI->verticalHeader()->hide();
    //ui->tableWidgetOrderSenderATI->setColumnWidth(0, std::max(ui->tableWidgetMakeOrder->columnWidth(0), ui->tableWidgetOrderSenderATI->columnWidth(0)));

}

//Функция поиска подстроки в строке
bool MainWindow::findSubStr(QString search, QString text) {
    int searchLen, textLen;
    searchLen = search.length();
    textLen = text.length();
    QString s = search, t = text;
    search = s.toLower();
    text = t.toLower();
    QString test;
    for(int i = 0; i < textLen; ++i) {
        test = text.mid(i, searchLen);
        if(test == search) {
            return true;
        }
    }
    return false;
}

//Функция поиска в векторе (для наряда) и вывод данных в таблицу
void MainWindow::findProductForOrder(QString searchText)
{
    QString currText = ui->comboBoxShetaOfSender->currentText();
    QVector <product> currentSchet;

    for(int i = 0; i < orderDataATI[currText].size(); ++i) {
        product tempProd = orderDataATI[currText][i];
        if(searchText.left(2) == "17") {
            if(findSubStr(searchText, tempProd.nomNumber)) {
                currentSchet.push_back(tempProd);
            }
        }else{
            if(findSubStr(searchText, tempProd.name)) {
                currentSchet.push_back(tempProd);
            }
        }
    }
    int len = currentSchet.size();

    ui->tableWidgetOrderSenderATI->clear();
    ui->tableWidgetOrderSenderATI->setRowCount(len);
    QStringList wordListNames;

    for (int i = 0; i < len; ++i)
    {


        ui->tableWidgetOrderSenderATI->setItem(i, 0, new QTableWidgetItem(currentSchet[i].name));
        ui->tableWidgetOrderSenderATI->setItem(i, 1, new QTableWidgetItem(currentSchet[i].nomNumber));
        ui->tableWidgetOrderSenderATI->setItem(i, 2, new QTableWidgetItem(currentSchet[i].measure));
        ui->tableWidgetOrderSenderATI->setItem(i, 3, new QTableWidgetItem(QString::number(currentSchet[i].category)));
        ui->tableWidgetOrderSenderATI->setItem(i, 4, new QTableWidgetItem(QString::number(currentSchet[i].count)));
        ui->tableWidgetOrderSenderATI->setItem(i, 5, new QTableWidgetItem("")); //Добавить дату выпуска
        ui->tableWidgetOrderSenderATI->setItem(i, 6, new QTableWidgetItem("")); //Добавить заводской номер

        //Добавление кнопки "Добавить"
        QPushButton *addBtn = new QPushButton("Добавить");
        addBtn->setFixedSize(128, 30);
        ui->tableWidgetOrderSenderATI->setIndexWidget(ui->tableWidgetOrderSenderATI->model()->index(i, 8), addBtn);
        //addBtn->setStyleSheet("background-color: rgb(255, 255, 255);");
        connect(addBtn, SIGNAL( clicked( bool ) ), SLOT( addBtnAction() ) );

        wordListNames.append(currentSchet[i].name + " | " + currentSchet[i].nomNumber);
    }

    QCompleter *completer = new QCompleter(wordListNames, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEditFindOrderItem->setCompleter(completer);

    QStringList headers;
    headers.append("Наименование");
    headers.append("Номенклатурный номер");
    headers.append("Ед. изм.");
    headers.append("Категория");
    headers.append("Количество");
    headers.append("Дата выпуска");
    headers.append("Заводской номер");
    headers.append("");
    headers.append("Действие");
    ui->tableWidgetOrderSenderATI->setHorizontalHeaderLabels(headers);

    ui->tableWidgetOrderSenderATI->resizeColumnsToContents();

    ui->tableWidgetOrderSenderATI->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
    //ui->tableWidgetOrderSenderATI->horizontalHeader()->setStretchLastSection(true);
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

//Удаление имущества из наряда
void MainWindow::on_pushButton_clicked()
{
    /*int i = ui->tableWidgetMakeOrder->currentRow();

    ui->tableWidgetOrderSenderATI->setRowCount(ui->tableWidgetOrderSenderATI->rowCount() + 1);
    int j = ui->tableWidgetOrderSenderATI->rowCount() - 1;

    ui->tableWidgetOrderSenderATI->setItem(j, 0, new QTableWidgetItem(ui->tableWidgetMakeOrder->item(i, 0)->text()));
    ui->tableWidgetOrderSenderATI->setItem(j, 1, new QTableWidgetItem(ui->tableWidgetMakeOrder->item(i, 1)->text()));
    ui->tableWidgetOrderSenderATI->setItem(j, 2, new QTableWidgetItem(ui->tableWidgetMakeOrder->item(i, 2)->text()));
    ui->tableWidgetOrderSenderATI->setItem(j, 3, new QTableWidgetItem(ui->tableWidgetMakeOrder->item(i, 3)->text()));
    ui->tableWidgetOrderSenderATI->setItem(j, 4, new QTableWidgetItem(ui->tableWidgetMakeOrder->item(i, 4)->text()));
    ui->tableWidgetOrderSenderATI->setItem(j, 5, new QTableWidgetItem("")); //Добавить дату выпуска
    ui->tableWidgetOrderSenderATI->setItem(j, 6, new QTableWidgetItem("")); //Добавить заводской номер

    for(int k = 0; k < tempOrderProducts.size(); ++k)
    {
        if(tempOrderProducts[k].name == ui->tableWidgetMakeOrder->item(i, 0)->text() && tempOrderProducts[k].category == ui->tableWidgetMakeOrder->item(i, 3)->text().toInt())
        {
            tempOrderProducts.remove(k);
            break;
        }
    }

    ui->tableWidgetMakeOrder->removeRow(i);*/
}

//Формирование наряда в Excel
void MainWindow::on_pushButtonDoneMakeOrder_clicked()
{
    if(redactFlag) {
        on_deleteOrder_clicked();
        ui->pushButtonDoneMakeOrder->setText("Сформировать");
        redactFlag = false;
    }
    Document readyOrder ("Order.xlsx"); //Создание файла Excel для наряда

    order tempOrd; // Создание временного объекта "Наряд" для переноса в БД и регистрации в журнале

    readyOrder.write(6,1,"НАРЯД № " + ui->lineEditOutputNumberOrder->text());
    tempOrd.name = "НАРЯД № " + ui->lineEditOutputNumberOrder->text();
    tempOrd.numberOutput = ui->lineEditOutputNumberOrder->text();

    readyOrder.write(3, 12, "В подтверждение распоряжения ТЛГ № " + ui->telefonogramm->text() + " от " + ui->dateEdit_2->text() + " г.");

    readyOrder.write(8, 11, ui->dateOutputCreateOrder->text());
    tempOrd.dateOutput = ui->dateOutputCreateOrder->text();

    readyOrder.write(9, 11, ui->validOrderDate->text());
    tempOrd.expirationDate = ui->validOrderDate->text();

    readyOrder.write(14, 1, ui->orderTarget->currentText().left(2));
    tempOrd.target = ui->orderTarget->currentText().left(2);

    readyOrder.write(14, 2, ui->osnovanie->text() + " от " + ui->dateEditBase->text());
    tempOrd.base = ui->osnovanie->text() + " от " + ui->dateEditBase->text();

    readyOrder.write(14, 5, ui->comboBoxOrderSender->currentText());
    tempOrd.senders.push_back(ui->comboBoxOrderSender->currentText());

    readyOrder.write(14, 7, ui->comboBoxOrderRecipient->currentText());
    tempOrd.recipients.push_back(ui->comboBoxOrderRecipient->currentText());

    //Добавление шаблона организации при формировании наряда с новым получателем
    QSqlQuery query(baseDB);
    if(!baseDB.open()) {
        qDebug() << baseDB.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    query.exec("SELECT name FROM organisations WHERE name = '" + ui->comboBoxOrderRecipient->currentText() + "'");
    qDebug() << query.lastError();
    query.next();
    QString debug = query.value(0).toString();
    if(query.value(0).toString() == ui->comboBoxOrderRecipient->currentText()) {

    }else{
        query.exec("INSERT INTO organisations (name) VALUES ('" + ui->comboBoxOrderRecipient->currentText() + "')");
        qDebug() << query.lastError();
    }
    baseDB.close();
    importOrgsAndHumans();

    tempOrd.sendType = ui->comboBoxTypeMoveOrder->currentText();
    readyOrder.write(120, 1, "Порядок отправки: " + tempOrd.sendType + ". Срок прибытия караула с приемщиком __________________ от воинской части ____________");

    readyOrder.write(122, 1, ui->comboBox->currentText());
    readyOrder.write(122, 4, ui->comboBox_6->currentText());
    readyOrder.write(122, 7, ui->comboBox_7->currentText());
    tempOrd.chief = ui->comboBox->currentText() + " " + ui->comboBox_6->currentText() + " " + ui->comboBox_7->currentText();

    readyOrder.write(125, 1, ui->comboBox_3->currentText());
    readyOrder.write(125, 4, ui->comboBox_5->currentText());
    readyOrder.write(125, 7, ui->comboBox_8->currentText());
    tempOrd.oficier = ui->comboBox_3->currentText() + " " + ui->comboBox_5->currentText() + " " + ui->comboBox_8->currentText();

    tempOrd.completion = 0;

    tempOrd.typeProduct = "ДО"; //Тип наряда ДО или ВДО

    //Очистка наряда
    for(int i = 0; i < 100; ++i)
    {
        for(int j = 2; j < 13; ++j)
        {
            readyOrder.write(18 + i, j, "");
        }
    }

    //Заполнение наряда
    int delIndex = 18;
    QVector <product> tempProdContain;
    for(int i = 0; i < ui->tableWidgetMakeOrder->rowCount(); ++i)
    {
        product tempProd;
        readyOrder.write(18 + i, 2, ui->tableWidgetMakeOrder->item(i, 0)->text());
        tempProd.name = ui->tableWidgetMakeOrder->item(i, 0)->text();

        readyOrder.write(18 + i, 3, ui->tableWidgetMakeOrder->item(i, 1)->text());
        tempProd.nomNumber = ui->tableWidgetMakeOrder->item(i, 1)->text();

        readyOrder.write(18 + i, 7, ui->tableWidgetMakeOrder->item(i, 2)->text());
        tempProd.measure = ui->tableWidgetMakeOrder->item(i, 2)->text();

        readyOrder.write(18 + i, 6, ui->tableWidgetMakeOrder->item(i, 3)->text());
        tempProd.category = ui->tableWidgetMakeOrder->item(i, 3)->text().toInt();

        readyOrder.write(18 + i, 8, ui->tableWidgetMakeOrder->item(i, 4)->text());
        tempProd.count = ui->tableWidgetMakeOrder->item(i, 4)->text().toDouble();

        readyOrder.write(18 + i, 4, ui->tableWidgetMakeOrder->item(i, 6)->text());
        tempProd.factoryNumber = ui->tableWidgetMakeOrder->item(i, 6)->text();

        readyOrder.write(18 + i, 5, ui->tableWidgetMakeOrder->item(i, 5)->text());
        tempProd.releaseDate = ui->tableWidgetMakeOrder->item(i, 5)->text();

        tempProdContain.push_back(tempProd);
        delIndex++;
    }
    tempOrd.items = tempProdContain;


    //Заполнение количества наименований

    QString numerals[10] = {"", "одно", "два", "три", "четыре", "пять", "шесть", "семь", "восемь", "девять"};
    QString tens[10] = {"", "", "двадцать", "тридцать", "сорок", "пятьдесят", "шестьдесят", "семьдесят", "восемьдесят", "девяносто"};
    QString expectations[10] = {"десять", "одиннадцать", "двенадцать", "тринадцать", "четырнадцать", "пятнадцать", "шестнадцать", "семнадцать", "восемнадцать", "девятнадцать"};
    QString words[10] = {"наименований", "наименование", "наименования", "наименования", "наименования", "наименований", "наименований", "наименований", "наименований", "наименований"};
    int rows = ui->tableWidgetMakeOrder->rowCount();
    QString rowNumber;

    if(rows < 10) rowNumber = "Итого: " + QString::number(rows) + " (" + numerals[rows] + ") " + words[rows];
    if(rows >= 10 && rows < 20) rowNumber = "Итого: " + QString::number(rows) + " (" +  expectations[rows % 10] + ") " + words[0];
    if(rows >= 20) rowNumber =  "Итого: " + QString::number(rows) + " (" + tens[rows / 10] + " " + numerals[rows % 10] + ") " + words[rows % 10];

    readyOrder.write(118,1, rowNumber);
    //Скрытие лишних строк
    for(int i = ui->tableWidgetMakeOrder->rowCount() + 18; i < 118; ++i)
    {
        readyOrder.setRowHidden(i,true);
    }

    //Внесение изменений в БД
    for(int i = 0; i < ui->tableWidgetMakeOrder->rowCount(); ++i)
    {
        QString name = ui->tableWidgetMakeOrder->item(i, 0)->text();
        QString nomNumber = ui->tableWidgetMakeOrder->item(i, 1)->text();
        double count = ui->tableWidgetMakeOrder->item(i, 4)->text().toDouble();
        QString tempSchet = ui->comboBoxShetaOfSender->currentText();

        if(!baseDB.open()) {
            qDebug() << baseDB.lastError().text();
        }else{
            qDebug() << "It's OK!";
        }

        QString queryText = "SELECT id FROM lic_scheta WHERE title = \"" + tempSchet + "\"";
        qDebug() << query.lastError();
        QSqlQuery query(senderDB);
        query.exec(queryText);
        qDebug() << query.lastError();
        query.next();
        QString id = query.value(0).toString();

        for(int rowMake = 0; rowMake < ui->tableWidgetMakeOrder->rowCount(); ++rowMake)
        {
            product tempProd;
            tempProd.name = ui->tableWidgetMakeOrder->item(rowMake, 0)->text();
            tempProd.nomNumber = ui->tableWidgetMakeOrder->item(rowMake, 1)->text();
            tempProd.measure = ui->tableWidgetMakeOrder->item(rowMake, 2)->text();
            tempProd.category = ui->tableWidgetMakeOrder->item(rowMake, 3)->text().toInt();
            tempProd.count = ui->tableWidgetMakeOrder->item(rowMake, 4)->text().toDouble();
            tempProd.releaseDate = ui->tableWidgetMakeOrder->item(rowMake, 5)->text();
            tempProd.factoryNumber = ui->tableWidgetMakeOrder->item(rowMake, 6)->text();

            //
            bool flag = false; //Флаг для фиксации наличия имущества в лицевом счете (true -  имущество необходимо обновить, false - имущество необходимо создать и отнять количество)
            for(int i = 0; i < orderDataATI[tempSchet].size(); ++i)
            {
                if(orderDataATI[tempSchet][i].name == tempProd.name && orderDataATI[tempSchet][i].nomNumber == tempProd.nomNumber && orderDataATI[tempSchet][i].category == tempProd.category)
                {
                    double count = orderDataATI[tempSchet][i].count;
                    QString cnt = QString::number(count, 'g', 10);
                    flag = true; // Имущество есть на лицевом счете
                    queryText = "UPDATE product_ati SET count = " + cnt + " WHERE name = \"" + tempProd.name + "\" AND nom_numb = \"" + tempProd.nomNumber + "\" AND category = " + QString::number(tempProd.category);
                    query.exec(queryText);
                    qDebug() << query.lastError() << endl << queryText;
                }
            }

            if(!flag) {
                QString Text = "INSERT INTO product_ati (name, nom_numb, measure, count, category, id_lic_scheta) VALUES (\"" + tempProd.name + "\", \"" + tempProd.nomNumber + "\", \"" + tempProd.measure + "\", \"" + QString::number(tempProd.count * (-1), 'g', 10) + "\", \"" + QString::number(tempProd.category) + "\", \"" + id + "\")";
                query.exec(Text);
                qDebug() << query.lastError() << endl << Text;
            }

        }

        baseDB.close();
    }

    //Сохранение и открытие наряда
    QString fileName = "Order-" + ui->lineEditOutputNumberOrder->text() + ".xlsx";
    fileName.replace("/", "-");
    QString orderDir = QDir::currentPath() + "/CreatedOrders/" + fileName;
    readyOrder.saveAs(orderDir);

    QString fileNameForUrl = orderDir;
    orderDir.replace("/", "\\");

    QDesktopServices::openUrl(QUrl::fromLocalFile(fileNameForUrl));

    //Наряд на основании распоряжения ВДО
    if(ui->checkBoxVDO->isChecked() == true){
        tempOrd.tlg = "ДО";
    }

    addOrderToDB(tempOrd); // Добавление в БД наряда

    ordersFromBD();

    on_pushButtonCancelMakeOrder_clicked(); //Очистка окон после создания распоряжения
}

//Нажатие кнопки далее
void MainWindow::on_pushButtonNextPrihod_3_clicked()
{
    ui->tabWidgetProvodPrihod_3->setCurrentIndex(1);
}

// Нажатие кнопки назад
void MainWindow::on_pushButtonBackMakeOrder_clicked()
{
    ui->tabWidgetProvodPrihod_3->setCurrentIndex(0);
}

// Нажатие кнопки отмена
void MainWindow::on_pushButtonCancelMakeOrder_clicked()
{
    //Очистка вкладки "Перечень МЦ"
    ui->tableWidgetMakeOrder->setRowCount(0);
    ui->tableWidgetOrderSenderATI->setRowCount(0);
    ui->comboBoxShetaOfSender->clear();
    ui->lineEditFindOrderItem->setText("");
    //Очистка вкладки "Основная информация"
    ui->comboBoxTypeDocPrihod_3->setCurrentIndex(0);
    ui->lineEditOutputNumberOrder->setText("");
    ui->osnovanie->setText("");
    ui->comboBoxOrderSender->setCurrentIndex(0);
    ui->comboBoxOrderRecipient->setCurrentIndex(0);
    ui->comboBoxTypeMoveOrder->setCurrentIndex(0);

    ui->tableWidgetOrders->resizeColumnsToContents();
    ui->tableWidgetOrders->horizontalHeader()->setStretchLastSection(true);
    ui->dateOutputCreateOrder->setDate(QDate::currentDate());
    ui->dateEdit_2->setDate(QDate::currentDate());
    ui->dateEditBase->setDate(QDate::currentDate());

    int currMonth = (ui->dateOutputCreateOrder->date().month() + 4) % 12, currYear = ui->dateOutputCreateOrder->date().month() > 8 ? ui->dateOutputCreateOrder->date().year() + 1 : ui->dateOutputCreateOrder->date().year();
    int currDay = ui->dateOutputCreateOrder->date().day();
    QDate currDate(currYear, currMonth, currDay);
    ui->validOrderDate->setDate(currDate);

    //Переключение на вкладку "Выписанные распоряжения"
    ui->tabWidgetOrders->setCurrentIndex(0);
}

//Двойной щелчок по элементу таблицы для добавления имущества в наряд
void MainWindow::on_tableWidgetOrderSenderATI_cellDoubleClicked(int row, int column)
{
    //Подготовка временного контейнера
    product temp;
    temp.name = ui->tableWidgetOrderSenderATI->item(row, 0)->text();
    temp.nomNumber = ui->tableWidgetOrderSenderATI->item(row, 1)->text();
    temp.measure = ui->tableWidgetOrderSenderATI->item(row, 2)->text();
    temp.category = ui->tableWidgetOrderSenderATI->item(row, 3)->text().toInt();
    temp.count = ui->tableWidgetOrderSenderATI->item(row, 4)->text().toDouble();

    //Открытие окна добавления имущества
    AddItem *addItem = new AddItem(row, temp,this);
    addItem->setModal(true);
    connect(addItem, SIGNAL(dataToMainWindow(int, product)), SLOT(dataFromAddItem(int, product)));
    addItem->exec();
}

//Действия при нажатии кнопки "Добавить"
void MainWindow::addBtnAction()
{
    int row = ui->tableWidgetOrderSenderATI->currentRow();
    on_tableWidgetOrderSenderATI_cellDoubleClicked(row, 0);
}

//Обновление данных в таблице и в базе данных после добавления имущества
void MainWindow::dataFromAddItem(int row, product item) {
    if(row == -1) {
        // Добавление имущества в наряд
        int rowIndex = ui->tableWidgetOrderVDO->rowCount();
        ui->tableWidgetOrderVDO->setRowCount(rowIndex + 1);
        ui->tableWidgetOrderVDO->setItem(rowIndex, 0, new QTableWidgetItem(item.name));
        ui->tableWidgetOrderVDO->setItem(rowIndex, 1, new QTableWidgetItem(item.nomNumber));
        ui->tableWidgetOrderVDO->setItem(rowIndex, 2, new QTableWidgetItem(item.measure));
        if(item.category != 0)
            ui->tableWidgetOrderVDO->setItem(rowIndex, 3, new QTableWidgetItem(QString::number(item.category)));
        else {
            ui->tableWidgetOrderVDO->setItem(rowIndex, 3, new QTableWidgetItem(""));
        }
        ui->tableWidgetOrderVDO->setItem(rowIndex, 4, new QTableWidgetItem(QString::number(item.count)));
        ui->tableWidgetOrderVDO->setItem(rowIndex, 5, new QTableWidgetItem(item.releaseDate));
        ui->tableWidgetOrderVDO->setItem(rowIndex, 6, new QTableWidgetItem(item.factoryNumber));

        //Создание кнопки "Удалить"
        QPushButton *deleteBtn = new QPushButton("Удалить");
        deleteBtn->setFixedSize(128, 30);
        //deleteBtn->setStyleSheet("background-color: rgb(255, 255, 255);");
        connect(deleteBtn, SIGNAL( clicked( bool ) ), SLOT(deleteItemFromTempOrderVDO() ) );

        ui->tableWidgetOrderVDO->setIndexWidget(ui->tableWidgetOrderVDO->model()->index(rowIndex, 7), deleteBtn);

        ui->tableWidgetOrderVDO->item(rowIndex, 1)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrderVDO->item(rowIndex, 2)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrderVDO->item(rowIndex, 3)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrderVDO->item(rowIndex, 4)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrderVDO->item(rowIndex, 5)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrderVDO->item(rowIndex, 6)->setTextAlignment(Qt::AlignCenter);
        //ui->tableWidgetOrderVDO->item(rowIndex, 7)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrderVDO->resizeColumnsToContents();

        //ui->tableWidgetOrderVDO->horizontalHeader()->setSectionResizeMode(8, QHeaderView::Stretch);

        //ui->tableWidgetOrderVDO->verticalHeader()->hide();
        ui->tableWidgetOrderVDO->horizontalHeader()->setStretchLastSection(true);
        //ui->tableWidgetOrderVDO->setColumnWidth(0, std::max(ui->tableWidgetOrderVDO->columnWidth(0), ui->tableWidgetOrderSenderATI->columnWidth(0)));
    }else{
        if(row == -2){
            row = ui->tableWidgetOrderSenderATI->rowCount();
            ui->tableWidgetOrderSenderATI->setRowCount(row + 1);
            ui->tableWidgetOrderSenderATI->setItem(row, 4, new QTableWidgetItem("0"));
            ui->tableWidgetOrderSenderATI->setItem(row, 5, new QTableWidgetItem(""));
            ui->tableWidgetOrderSenderATI->setItem(row, 6, new QTableWidgetItem(""));

            //Добавление кнопки "Добавить"
            QPushButton *addBtn = new QPushButton("Добавить");
            addBtn->setFixedSize(128, 30);
            ui->tableWidgetOrderSenderATI->setIndexWidget(ui->tableWidgetOrderSenderATI->model()->index(row, 8), addBtn);
            //addBtn->setStyleSheet("background-color: rgb(255, 255, 255);");
            connect(addBtn, SIGNAL( clicked( bool ) ), SLOT( addBtnAction() ) );
        }
        ui->tableWidgetOrderSenderATI->setItem(row, 0, new QTableWidgetItem(item.name));
        ui->tableWidgetOrderSenderATI->setItem(row, 1, new QTableWidgetItem(item.nomNumber));
        ui->tableWidgetOrderSenderATI->setItem(row, 2, new QTableWidgetItem(item.measure));
        if(item.category != 0)
            ui->tableWidgetOrderSenderATI->setItem(row, 3, new QTableWidgetItem(QString::number(item.category)));
        else {
            ui->tableWidgetOrderSenderATI->setItem(row, 3, new QTableWidgetItem(""));
        }
        double cnt = ui->tableWidgetOrderSenderATI->item(row, 4)->text().toDouble() - item.count;
        ui->tableWidgetOrderSenderATI->setItem(row, 4, new QTableWidgetItem(QString::number(cnt)));

        ui->tableWidgetOrderSenderATI->item(row, 1)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrderSenderATI->item(row, 2)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrderSenderATI->item(row, 3)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrderSenderATI->item(row, 4)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrderSenderATI->item(row, 5)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetOrderSenderATI->item(row, 6)->setTextAlignment(Qt::AlignCenter);

        // Внесение изменений во временный контейнер для хранения имущества по лицевым счетам
        QString tempSchet = ui->comboBoxShetaOfSender->currentText();
        for(int i = 0; i < orderDataATI[tempSchet].size(); ++i)
        {
            if(orderDataATI[tempSchet][i].name == item.name && orderDataATI[tempSchet][i].nomNumber == item.nomNumber)
            {
                orderDataATI[tempSchet][i].count -= item.count;
            }
        }

        // Добавление имущества в наряд
        int rowIndex = ui->tableWidgetMakeOrder->rowCount();
        ui->tableWidgetMakeOrder->setRowCount(rowIndex + 1);
        ui->tableWidgetMakeOrder->setItem(rowIndex, 0, new QTableWidgetItem(item.name));
        ui->tableWidgetMakeOrder->setItem(rowIndex, 1, new QTableWidgetItem(item.nomNumber));
        ui->tableWidgetMakeOrder->setItem(rowIndex, 2, new QTableWidgetItem(item.measure));
        if(item.category != 0)
            ui->tableWidgetMakeOrder->setItem(rowIndex, 3, new QTableWidgetItem(QString::number(item.category)));
        else {
            ui->tableWidgetMakeOrder->setItem(rowIndex, 3, new QTableWidgetItem(""));
        }
        ui->tableWidgetMakeOrder->setItem(rowIndex, 4, new QTableWidgetItem(QString::number(item.count)));
        ui->tableWidgetMakeOrder->setItem(rowIndex, 5, new QTableWidgetItem(item.releaseDate));
        ui->tableWidgetMakeOrder->setItem(rowIndex, 6, new QTableWidgetItem(item.factoryNumber));

        //Создание кнопки "Удалить"
        QPushButton *deleteBtn = new QPushButton("Удалить");
        deleteBtn->setFixedSize(128, 30);
        //deleteBtn->setStyleSheet("background-color: rgb(255, 255, 255);");
        connect(deleteBtn, SIGNAL( clicked( bool ) ), SLOT( deleteItemFromTempOrder() ) );

        ui->tableWidgetMakeOrder->setIndexWidget(ui->tableWidgetMakeOrder->model()->index(rowIndex, 8), deleteBtn);

        //connect(addBtn, SIGNAL( clicked( bool ) ), SLOT( addBtnAction() ) );
        //ui->tableWidgetMakeOrder->item(rowIndex, 0)->setTextAlignment(Qt::AlignLeft);
        ui->tableWidgetMakeOrder->item(rowIndex, 1)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetMakeOrder->item(rowIndex, 2)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetMakeOrder->item(rowIndex, 3)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetMakeOrder->item(rowIndex, 4)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetMakeOrder->item(rowIndex, 5)->setTextAlignment(Qt::AlignCenter);
        ui->tableWidgetMakeOrder->item(rowIndex, 6)->setTextAlignment(Qt::AlignCenter);
        //ui->tableWidgetMakeOrder->item(rowIndex, 7)->setTextAlignment(Qt::AlignCenter);

        ui->tableWidgetMakeOrder->resizeColumnsToContents();
        //ui->tableWidgetMakeOrder->horizontalHeader()->setStretchLastSection(true);
        ui->tableWidgetMakeOrder->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
        ui->tableWidgetMakeOrder->verticalHeader()->hide();
        ui->tableWidgetMakeOrder->setColumnWidth(0, std::max(ui->tableWidgetMakeOrder->columnWidth(0), ui->tableWidgetOrderSenderATI->columnWidth(0)));
    }
}

//Добавление в формируемый наряд имущества без номенклатуры или без учета
void MainWindow::on_pushButtonFindPrihod_3_clicked()
{
    if(ui->comboBoxShetaOfSender->currentIndex() != -1) {
        addItemBN *add = new addItemBN(this);
        add->setModal(true);
        connect(add, SIGNAL(dataToMainWindow(int, product)), SLOT(dataFromAddItem(int, product)));
        add->exec();
    }else{
        QMessageBox msg;
        msg.setWindowTitle("Внимание!");
        msg.setText("Выберите лицевой счет!");
        msg.setIcon(QMessageBox::Warning);
        msg.exec();
    }

}

//Подключение выбранной воинской части (базы данных)
void MainWindow::on_selectVCh_activated(const QString &arg1)
{
    currDB = DataBases[arg1];
    importBD();
    importOrgsAndHumans();
    docsFromBD();

    if(arg1 == "Все в/ч") {
        //Заполнение контейнера АТИ данными из БД
        dataATI.clear();
        for (int i = 1; i < ui->selectVCh->count(); ++i) {
            QString vCH = ui->selectVCh->itemText(i);
            currDB = DataBases[vCH];

            QString queryText = "SELECT * FROM lic_scheta";
            QSqlQuery query(currDB), queryProduct(currDB);
            query.exec(queryText);
            qDebug() << query.lastError();
            while(query.next())
            {
                //Считывание лицевых счетов
                QString id = query.value(0).toString();
                QString schet = vCH + " || " + query.value(1).toString();
                QVector <product> currentSchet;

                queryText = "SELECT * FROM product_ati WHERE id_lic_scheta = " + id;
                queryProduct.exec(queryText);
                qDebug() << queryProduct.lastError();

                while(queryProduct.next())
                {
                    //Считывание АТИ из лицевого счета
                    product temp;
                    temp.name = queryProduct.value(1).toString();
                    temp.nomNumber = queryProduct.value(2).toString();
                    temp.price = queryProduct.value(3).toDouble();
                    temp.measure = queryProduct.value(4).toString();
                    temp.count = queryProduct.value(5).toDouble();
                    temp.factoryNumber = queryProduct.value(6).toString();
                    temp.category = queryProduct.value(7).toInt();
                    temp.releaseDate = queryProduct.value(8).toString();

                    currentSchet.push_back(temp);
                }

                dataATI.insert(schet, currentSchet);
            }

            //Заполнение комбобокса лицевыми счетами из временного контейнера QMap
            QMapIterator <QString, QVector <product>> it(dataATI);
            ui->comboBoxScheta->clear();
            ui->comboBoxScheta->addItem("Все лицевые счета");
            while(it.hasNext()) {
                it.next();
                ui->comboBoxScheta->addItem(it.key());
            }

        on_comboBoxScheta_activated(ui->comboBoxScheta->itemText(0));
        ordersFromBD();
       }
    }
}

//Поиск в выбранном лицевом счете
void MainWindow::on_findInAtiInSkladButton_clicked()
{
    QString searchText = ui->lineEditFindInSklad->text();
    QString currText = ui->comboBoxScheta->currentText();
    QVector <QPair<product, QString>> currentSchet;

    if(currText == "Все лицевые счета"){
        for(QMap <QString, QVector<product>>::iterator it = dataATI.begin(); it != dataATI.end(); it++) {
            for(int i = 0; i < it.value().size(); ++i) {
                product tempProd = it.value()[i];
                if(searchText.left(2) == "17") {
                    if(findSubStr(searchText, tempProd.nomNumber)) {
                        //QPair <product, QString> = qMakePair
                        currentSchet.push_back(qMakePair(tempProd, it.key()));
                    }
                }else{
                    if(findSubStr(searchText, tempProd.name)) {
                        currentSchet.push_back(qMakePair(tempProd, it.key()));
                    }
                }
            }
        }

    }else{
        for(int i = 0; i < dataATI[currText].size(); ++i) {
            product tempProd = dataATI[currText][i];
            if(searchText.left(2) == "17") {
                if(findSubStr(searchText, tempProd.nomNumber)) {
                    currentSchet.push_back(qMakePair(tempProd, currText));
                }
            }else{
                if(findSubStr(searchText, tempProd.name)) {
                    currentSchet.push_back(qMakePair(tempProd, currText));
                }
            }
        }
    }
    int len = currentSchet.size();

    //ui->tableAccount->clear();
    //ui->tableAccount->setRowCount(len);
    QStringList wordListNames;

    QStandardItemModel *modelAccount = new QStandardItemModel();

        for (int i = 0; i < len; ++i)
    {
        /*ui->tableAccount->setItem(i, 0, new QTableWidgetItem(currentSchet[i].name));
        ui->tableAccount->setItem(i, 1, new QTableWidgetItem(currentSchet[i].nomNumber));
        ui->tableAccount->setItem(i, 2, new QTableWidgetItem(currentSchet[i].measure));
        ui->tableAccount->setItem(i, 3, new QTableWidgetItem(QString::number(currentSchet[i].category)));
        ui->tableAccount->setItem(i, 4, new QTableWidgetItem(QString::number(currentSchet[i].count)));
        ui->tableAccount->setItem(i, 5, new QTableWidgetItem(QString::number(currentSchet[i].price)));*/

        QList<QStandardItem*> lst;
        QStandardItem *item = new QStandardItem(currentSchet[i].first.name);
        QStandardItem *item1 = new QStandardItem(currentSchet[i].first.nomNumber);
        QStandardItem *item2 = new QStandardItem(currentSchet[i].first.measure);
        QStandardItem *item3 = new QStandardItem(QString::number(currentSchet[i].first.category));
        QStandardItem *item4 = new QStandardItem(QString::number(currentSchet[i].first.count));
        QStandardItem *item5 = new QStandardItem(currentSchet[i].second);

        lst <<item<<item1<<item2<<item3<<item4<<item5;
        modelAccount->appendRow(lst);

        wordListNames.append(currentSchet[i].first.name + " | " + currentSchet[i].first.nomNumber);
    }
        modelAccount->setHeaderData(0,Qt::Horizontal, "Наименование");
        modelAccount->setHeaderData(1,Qt::Horizontal, "Ном. номер");
        modelAccount->setHeaderData(2,Qt::Horizontal, "Ед. изм.");
        modelAccount->setHeaderData(3,Qt::Horizontal, "Категория");
        modelAccount->setHeaderData(4,Qt::Horizontal, "Количество");
        modelAccount->setHeaderData(5,Qt::Horizontal, "Лицевой счет");

    QCompleter *completer = new QCompleter(wordListNames, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    ui->lineEditFindOrderItem->setCompleter(completer);

    /*QStringList headers;
    headers.append("Наименование");
    headers.append("Номенклатурный номер");
    headers.append("Ед. изм.");
    headers.append("Категория");
    headers.append("Количество");
    headers.append("Лицевой счет");
    ui->tableAccount->setHorizontalHeaderLabels(headers);
*/

    ui->tableAccount->setModel(modelAccount);
    ui->tableAccount->resizeColumnsToContents();
    ui->tableAccount->horizontalHeader()->setStretchLastSection(true);
    ui->tableAccount->resizeColumnsToContents();
    ui->tableAccount->show();

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

void MainWindow::on_comboBoxFindtypeAti_activated(const QString &arg1)
{

}

// Сигнал при изменении исходящего номера распоряжения (проверка наличия идентичного наряда)
void MainWindow::on_lineEditOutputNumberOrder_textChanged(const QString &arg1)
{
    QPalette pal = ui->lineEditOutputNumberOrder->palette();
    pal.setColor(QPalette::Text, Qt::black);
    ui->lineEditOutputNumberOrder->setPalette(pal);
    ui->pushButtonNextPrihod_3->setEnabled(true);
    ui->tab_20->setEnabled(true);
    if(!redactFlag){
        for(int i = 0; i < orders.size(); ++i) {
            if(orders[i].numberOutput == arg1) {
                pal = ui->lineEditOutputNumberOrder->palette();
                pal.setColor(QPalette::Text, Qt::red);
                ui->lineEditOutputNumberOrder->setPalette(pal);
                ui->pushButtonNextPrihod_3->setEnabled(false);
                ui->tab_20->setEnabled(false);

                QMessageBox msg;
                msg.setWindowTitle("Внимание!");
                msg.setText("Распоряжение с исходящим № " + arg1 + " уже существует!");
                msg.setIcon(QMessageBox::Warning);
                msg.exec();

                return;
            }
        }
    }
}

// Действия по нажатию кнопки "Удалить"
void MainWindow::deleteItemFromTempOrder()
{
    int rowMake = ui->tableWidgetMakeOrder->currentRow(); //Номер строки во временной таблице наряда
    product tempProd;

    tempProd.name = ui->tableWidgetMakeOrder->item(rowMake, 0)->text();
    tempProd.nomNumber = ui->tableWidgetMakeOrder->item(rowMake, 1)->text();
    tempProd.measure = ui->tableWidgetMakeOrder->item(rowMake, 2)->text();
    tempProd.category = ui->tableWidgetMakeOrder->item(rowMake, 3)->text().toInt();
    tempProd.count = ui->tableWidgetMakeOrder->item(rowMake, 4)->text().toDouble();
    tempProd.releaseDate = ui->tableWidgetMakeOrder->item(rowMake, 5)->text();
    tempProd.factoryNumber = ui->tableWidgetMakeOrder->item(rowMake, 6)->text();

    ui->tableWidgetMakeOrder->removeRow(rowMake);

    for(int i = 0; i < ui->tableWidgetOrderSenderATI->rowCount(); ++i)
    {
        if(ui->tableWidgetOrderSenderATI->item(i, 0)->text() == tempProd.name && ui->tableWidgetOrderSenderATI->item(i, 1)->text() == tempProd.nomNumber)
        {
            double cnt = ui->tableWidgetOrderSenderATI->item(i, 4)->text().toDouble() + tempProd.count;
            ui->tableWidgetOrderSenderATI->item(i, 4)->setText(QString::number(cnt, 'g', 10));
        }
    }

    // Внесение изменений во временный контейнер для хранения имущества по лицевым счетам
    QString tempSchet = ui->comboBoxShetaOfSender->currentText();
    for(int i = 0; i < orderDataATI[tempSchet].size(); ++i)
    {
        if(orderDataATI[tempSchet][i].name == tempProd.name && orderDataATI[tempSchet][i].nomNumber == tempProd.nomNumber)
        {
            orderDataATI[tempSchet][i].count += tempProd.count;
        }
    }
}

void MainWindow::on_comboBoxOrderRecipient_activated(const QString &arg1)
{

}

void MainWindow::on_dateEditBeginOrder_userDateChanged(const QDate &date)
{
    dateSort(ui->dateEditBeginOrder->date(), ui->dateEditEndOrder->date());
}

void MainWindow::on_dateEditEndOrder_userDateChanged(const QDate &date)
{
    dateSort(ui->dateEditBeginOrder->date(), ui->dateEditEndOrder->date());
}

//Отбор нарядов по дате
void MainWindow::dateSort(const QDate &dateBegin, const QDate &dateEnd)
{
    for(int i = 0; i < ui->tableWidgetOrders->rowCount();++i)
    {
        //QString dt, day, month, year;
        //dt = ui->tableWidgetOrders->item(i, 3)->text();
        //day = dt.left(2);
        //month = dt.mid(3,2);
        //year = dt.right(4);

        QVariant var = ui->tableWidgetOrders->item(i, 3)->data(Qt::DisplayRole);
        QDate orderDate = var.toDate();

        if(orderDate >= dateBegin && orderDate <= dateEnd)
        {
            ui->tableWidgetOrders->showRow(i);
        }else{
            ui->tableWidgetOrders->hideRow(i);      }
    }
}

//Отбор нарядов по отправителям
void MainWindow::on_comboBoxSenders_2_activated(const QString &arg1)
{
    if(arg1 == "Все отправители")
    {
        for(int i = 0; i < ui->tableWidgetOrders->rowCount();++i)
        {
            ui->tableWidgetOrders->showRow(i);
        }
    }else
    {
        for(int i = 0; i < ui->tableWidgetOrders->rowCount();++i)
        {
            if(ui->tableWidgetOrders->item(i, 6)->text() == arg1)
            {
                ui->tableWidgetOrders->showRow(i);
            }else
            {
                ui->tableWidgetOrders->hideRow(i);
            }
        }
    }
}

//Отбор нарядов по получателям
void MainWindow::on_comboBoxTypeMovePrihod_2_activated(const QString &arg1)
{
    if(arg1 == "Все получатели")
    {
        for(int i = 0; i < ui->tableWidgetOrders->rowCount();++i)
        {
            ui->tableWidgetOrders->showRow(i);
        }
    }else
    {
        for(int i = 0; i < ui->tableWidgetOrders->rowCount();++i)
        {
            if(ui->tableWidgetOrders->item(i, 7)->text() == arg1)
            {
                ui->tableWidgetOrders->showRow(i);
            }else
            {
                ui->tableWidgetOrders->hideRow(i);
            }
        }
    }
}

//Отбор нарядов по операциям
void MainWindow::on_comboBoxTypeProductPrihod_2_activated(const QString &arg1)
{
    if(arg1 == "Все операции")
    {
        for(int i = 0; i < ui->tableWidgetOrders->rowCount();++i)
        {
            ui->tableWidgetOrders->showRow(i);
        }
    }else
    {
        for(int i = 0; i < ui->tableWidgetOrders->rowCount();++i)
        {
            if(ui->tableWidgetOrders->item(i, 4)->text() == arg1.left(2))
            {
                ui->tableWidgetOrders->showRow(i);
            }else
            {
                ui->tableWidgetOrders->hideRow(i);
            }
        }
    }
}

void MainWindow::on_pushButton_5_clicked()
{

}

void MainWindow::on_pushButtonRightPrihod_clicked()
{

}

//Сохранение скана наряда
void MainWindow::downloadBtnAction()
{
    int  row = ui->tableWidgetOrders->currentRow();
    QString dirName = ui->tableWidgetOrders->item(row, 2)->text().replace("/", "-");

    QDir scanDir (QDir::currentPath() + "/CreatedOrders/");
    scanDir.mkdir("Scans " + dirName);
    QStringList scans = QFileDialog::getOpenFileNames(this, "Выберите файл(-ы) скана", "", "*.pdf");

    for(int i = 0; i < scans.size(); ++i)
    {
        QFile tempScan(scans[i]);

        //tempScan.rename("Scan " + dirName + "(" + QString::number(i) + ")");
        tempScan.copy(QDir::currentPath() + "/CreatedOrders/" + "Scans " + dirName + "/Scan " + dirName + "(" + QString::number(i) + ").pdf");
    }
}

//Открытие окна просмотра сканов нарядов
void MainWindow::openBtnAction()
{
    int  row = ui->tableWidgetOrders->currentRow();
    QString dirName = ui->tableWidgetOrders->item(row, 2)->text().replace("/", "-");

    QDir scanDir (QDir::currentPath() + "/CreatedOrders/");
    scanDir.mkdir("Scans " + dirName);

    /*scanViewer *viewer = new scanViewer(QDir::currentPath() + "/CreatedOrders/" + "Scans " + dirName, this);
    viewer->setModal(true);
    viewer->exec();*/

    scanDir.setPath(QDir::currentPath() + "/CreatedOrders/" + "Scans " + dirName);
    QStringList scans = scanDir.entryList();
    if(scans.size() < 3)
    {
        QMessageBox msg;
        msg.setWindowTitle("Внимание!");
        msg.setText("Загруженных сканов нет!");
        msg.setIcon(QMessageBox::Warning);
        msg.exec();
    }else{


        QString fileNameForUrl = scanDir.path() + "/" + scans[2];

        //QProcess::execute("PDFXCview.exe \"" + fileNameForUrl + "\"");

        QDesktopServices::openUrl(QUrl::fromLocalFile(fileNameForUrl));
    }
}

void MainWindow::on_tableWidgetOrders_cellDoubleClicked(int row, int column)
{
    openOrderDetails();
}

void MainWindow::on_lineEditFindPrihod_2_textChanged(const QString &arg1)
{
        QString text = arg1;
        //Скрытие всех строк
        for(int k = 0; k < ui->tableWidgetOrders->rowCount(); ++k)
        {
            ui->tableWidgetOrders->hideRow(k);
        }

        //Поиск фразы среди элементов
        for(int i = 0; i < orders.size(); ++i)
        {
            for(int j = 0; j < orders[i].items.size(); ++j)
            {
                //Совпадение наименования имущества
                if(findSubStr(text, orders[i].items[j].name) || findSubStr(text, orders[i].items[j].nomNumber))
                {
                    for(int k = 0; k < ui->tableWidgetOrders->rowCount(); ++k)
                    {
                        if(ui->tableWidgetOrders->item(k, 2)->text() == orders[i].numberOutput)
                        {
                            ui->tableWidgetOrders->showRow(k);
                        }

                    }
                }
                for(int k = 0; k < ui->tableWidgetOrders->rowCount(); ++k)
                {
                    //Совпадение исходящего номера
                    if(findSubStr(text, ui->tableWidgetOrders->item(k, 2)->text()))
                    {
                        ui->tableWidgetOrders->showRow(k);
                    }
                }
            }
        }
}


////////////////////////////////////////////////////////////////////////////////////////////////////// --- ПРИХОДНЫЕ ДОКУМЕНТЫ --- ////////////////////////////////////////////////////////////////////////////////////////////////

//Добавление строки в таблицу МЦ приходные документы
void MainWindow::on_pushButtonAddLinePrihod_clicked()
{
    ui->tableWidgetProductsPrihodDoc->insertRow(ui->tableWidgetProductsPrihodDoc->rowCount());
}

//Удаление строки в таблицу МЦ приходные документы
void MainWindow::on_pushButtonRemoveLinePrihod_clicked()
{
    ui->tableWidgetProductsPrihodDoc->removeRow(ui->tableWidgetProductsPrihodDoc->currentRow());
}

//Добавление строки в таблицу МЦ расходные документы
void MainWindow::on_pushButtonAddLineRashod_clicked()
{
    ui->tableWidgetProductsRashodDoc->insertRow(ui->tableWidgetProductsRashodDoc->rowCount());
}

//Удаление строки в таблицу МЦ расходные документы
void MainWindow::on_pushButtonRemoveLineRashod_clicked()
{
    ui->tableWidgetProductsRashodDoc->removeRow(ui->tableWidgetProductsRashodDoc->currentRow());
}

//Изменение шаблонов в зависимости от Внешнего или Внутреннего перемещения
void MainWindow::on_comboBoxTypeMoveProvodPrihod_activated(int index)
{
    ui->dateInputPrihod->setDate(QDate::currentDate());
    ui->dateOutputPrihod->setDate(QDate::currentDate());

    ui->comboBoxOrderPrihod->clear();
    for(int i = 0; i < orders.size(); ++i) {
        ui->comboBoxOrderPrihod->addItem(orders[i].name);
    }

    if(index == 0) {
        //ui->comboBoxRecipientPrihod->clear();
        ui->comboBoxSenderPrihod->clear();
    }

    if(index == 1) {
        //ui->comboBoxRecipientPrihod->clear();
        ui->comboBoxSenderPrihod->clear();
        ui->comboBoxTypeDocPrihod->clear();
        //ui->comboBoxRecipientPrihod->addItem("Авиационно-технический склад");
        ui->comboBoxTypeDocPrihod->addItem("Акт приема-передачи ОС");
        ui->comboBoxTypeDocPrihod->addItem("Накл. на внутр. перем. объектов НФА");
        tempDoc.type = 11; // 1 - Приходные; 1 - Внешнее перемещение
    }

    if(index == 2) {
        //ui->comboBoxRecipientPrihod->clear();
        ui->comboBoxSenderPrihod->clear();
        ui->comboBoxTypeDocPrihod->clear();
        //ui->comboBoxRecipientPrihod->addItem("Авиационно-технический склад");
        QMapIterator <QString, QVector <product>> it(dataATI);
        while (it.hasNext()) {
            it.next();
            ui->comboBoxSenderPrihod->addItem(it.key());
        }
        ui->comboBoxTypeDocPrihod->addItem("Накл. на внутр. перем. объектов НФА");
        tempDoc.type = 12; // 1 - Приходные; 2 - Внутреннее перемещение
    }
}

//Предварительное сохранение основных данных документа и переход на вкладку перечня МЦ
void MainWindow::on_pushButtonNextPrihod_clicked()
{
    tempDoc.clear();
    tempDoc.name = ui->comboBoxTypeDocPrihod->currentText();
    tempDoc.numberInput = ui->lineEditInputNumberPrihod->text();
    tempDoc.dateInput = ui->dateInputPrihod->date();
    tempDoc.numberOutput = ui->lineEditOutputNumberPrihod->text();
    tempDoc.dateOutput = ui->dateOutputPrihod->date();
    tempDoc.base = ui->comboBoxOrderPrihod->currentText();
    tempDoc.senderName = ui->comboBoxSenderPrihod->currentText();
    tempDoc.recipientName = ui->comboBoxRecipientPrihod->currentText();
    ui->tabWidgetProvodPrihod->setCurrentIndex(1);
}

// Возврат на вкладку основных данных документа
void MainWindow::on_pushButtonBackPrihod_clicked()
{
    ui->tabWidgetProvodPrihod->setCurrentIndex(1);
}

// Проведение приходного документа
void MainWindow::on_pushButtonDonePrihod_clicked()
{
    for(int i = 0; i < ui->tableWidgetProductsRashodDoc->rowCount(); ++i) {
        product tmp;
        tmp.name = ui->tableWidgetProductsRashodDoc->item(i,0)->text();
        tmp.nomNumber = ui->tableWidgetProductsRashodDoc->item(i,1)->text();
        tmp.measure = ui->tableWidgetProductsRashodDoc->item(i,2)->text();
        tmp.count = ui->tableWidgetProductsRashodDoc->item(i,3)->text().toDouble();
        tmp.category = ui->tableWidgetProductsRashodDoc->item(i,4)->text().toInt();
        tmp.factoryNumber =ui->tableWidgetProductsRashodDoc->item(i,5)->text();
        tmp.price = ui->tableWidgetProductsRashodDoc->item(i,6)->text().toDouble();
        tmp.releaseDate = ui->tableWidgetProductsRashodDoc->item(i,6)->text();

        tempDoc.items.push_back(tmp);
    }
        //ui->tableWidgetPrihod->insertRow(ui->tableWidget->rowCount());
        QSqlQuery query(currDB);
        query.exec("INSERT INTO docs (name, numberOutput, dateOutput, numberInput, dateInput, base, sender, recipient, type) VALUES "
                   "(\"" + tempDoc.name + "\", \"" + tempDoc.numberOutput + "\", \"" + tempDoc.dateOutput.toString() + "\", \"" + tempDoc.numberInput + "\", \"" +
                   tempDoc.dateInput.toString() + "\", \"" + tempDoc.base + "\", \"" + tempDoc.senderName + "\", \"" + tempDoc.recipientName + "\", \"" + QString::number(tempDoc.type) + "\")");
        qDebug() << query.lastError();
        docsFromBD();
}

// Забираем документы из БД
void MainWindow::docsFromBD()
{
    QSqlQuery query(currDB);
    query.exec("SELECT * FROM docs");
    qDebug() << query.lastError();
    ui->tableWidgetPrihod->setRowCount(0);
    ui->tableWidgetRashod->setRowCount(0);
    while(query.next()) {
        tempDoc.clear();

        tempDoc.name = query.value(1).toString();
        tempDoc.numberOutput = query.value(2).toString();
        tempDoc.dateOutput = QDate::fromString(query.value(3).toString());
        tempDoc.numberInput = query.value(4).toString();
        tempDoc.dateInput = QDate::fromString(query.value(5).toString());
        tempDoc.base = query.value(6).toString();
        tempDoc.senderName = query.value(9).toString();
        tempDoc.recipientName = query.value(10).toString();
        tempDoc.type = query.value(11).toInt();

        if(tempDoc.type == 11 || tempDoc.type == 12) {
            ui->tableWidgetPrihod->insertRow(ui->tableWidgetPrihod->rowCount());
            int i = ui->tableWidgetPrihod->rowCount() - 1;

            ui->tableWidgetPrihod->setItem(i, 0, new QTableWidgetItem(tempDoc.name + " № " + tempDoc.numberOutput + " от " + tempDoc.dateOutput.toString("dd.MM.yyyy")));
            ui->tableWidgetPrihod->setItem(i, 1, new QTableWidgetItem(tempDoc.numberOutput));
            ui->tableWidgetPrihod->setItem(i, 2, new QTableWidgetItem(tempDoc.dateOutput.toString("dd.MM.yyyy")));
            ui->tableWidgetPrihod->setItem(i, 3, new QTableWidgetItem(tempDoc.senderName));
            ui->tableWidgetPrihod->setItem(i, 4, new QTableWidgetItem(tempDoc.recipientName));
            ui->tableWidgetPrihod->setItem(i, 5, new QTableWidgetItem(tempDoc.base));

            QPushButton* downloadBtn = new QPushButton("");
            QPushButton* openBtn = new QPushButton("");
            QIcon dwn, opn;
            dwn.addFile(":/new/icons/icons/downloadScan.png");
            opn.addFile(":/new/icons/icons/openScan.png");
            downloadBtn->setIcon(dwn);
            openBtn->setIcon(opn);

            downloadBtn->setFlat(true);
            openBtn->setFlat(true);

            ui->tableWidgetPrihod->setIndexWidget(ui->tableWidgetPrihod->model()->index(i, 6), downloadBtn);
            ui->tableWidgetPrihod->setIndexWidget(ui->tableWidgetPrihod->model()->index(i, 7), openBtn);

            ui->tableWidgetPrihod->resizeColumnsToContents();
        }else if (tempDoc.type == 21 || tempDoc.type == 22) {
            ui->tableWidgetRashod->insertRow(ui->tableWidgetRashod->rowCount());
            int i = ui->tableWidgetRashod->rowCount();

            ui->tableWidgetRashod->setItem(i, 0, new QTableWidgetItem(tempDoc.name + " № " + tempDoc.numberOutput + " от " + tempDoc.dateOutput.toString("dd.MM.yyyy")));
            ui->tableWidgetRashod->setItem(i, 1, new QTableWidgetItem(tempDoc.numberOutput));
            ui->tableWidgetRashod->setItem(i, 2, new QTableWidgetItem(tempDoc.dateOutput.toString("dd.MM.yyyy")));
            ui->tableWidgetRashod->setItem(i, 3, new QTableWidgetItem(tempDoc.senderName));
            ui->tableWidgetRashod->setItem(i, 4, new QTableWidgetItem(tempDoc.recipientName));
            ui->tableWidgetRashod->setItem(i, 5, new QTableWidgetItem(tempDoc.base));

            QPushButton* downBtn = new QPushButton("");
            QPushButton* opnBtn = new QPushButton("");
            QIcon dwn, opn;
            dwn.addFile(":/new/icons/icons/downloadScan.png");
            opn.addFile(":/new/icons/icons/openScan.png");
            downBtn->setIcon(dwn);
            opnBtn->setIcon(opn);

            downBtn->setFlat(true);
            opnBtn->setFlat(true);

            ui->tableWidgetRashod->setIndexWidget(ui->tableWidgetRashod->model()->index(i, 6), downBtn);
            ui->tableWidgetRashod->setIndexWidget(ui->tableWidgetRashod->model()->index(i, 7), opnBtn);
            ui->tableWidgetRashod->resizeColumnsToContents();
        }
    }
}
//Поиск среди номенклатора для добавления в распоряжение ВДО
void MainWindow::on_lineEdit_3_textChanged(const QString &arg1)
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

// Двойной клик по номенклатору для добавления имущества в распоряжение ВДО
void MainWindow::on_tableWidgetOrderNomkl_cellDoubleClicked(int row, int column)
{
    //Подготовка временного контейнера
    /*product temp;
    temp.name = ui->tableWidgetOrderNomkl->item(row, 0)->text();
    temp.nomNumber = ui->tableWidgetOrderNomkl->item(row, 1)->text();
    temp.measure = ui->tableWidgetOrderNomkl->item(row, 2)->text();
    temp.category = 1;
    temp.count = 0;

    //Открытие окна добавления имущества
    AddItem *addItem = new AddItem(-1, temp,this);
    addItem->setModal(true);
    connect(addItem, SIGNAL(dataToMainWindow(int, product)), SLOT(dataFromAddItem(int, product)));
    addItem->exec();*/
}

//Действие по нажатию кнопки "Удалить" в перечне распоряжения ВДО
void MainWindow::deleteItemFromTempOrderVDO() {
    int rowMake = ui->tableWidgetOrderVDO->currentRow(); //Номер строки во временной таблице наряда
    ui->tableWidgetOrderVDO->removeRow(rowMake);
}

// Двойной клик по номенклатору для добавления имущества в распоряжение ВДО
void MainWindow::on_tableViewNomklVDO_doubleClicked(const QModelIndex &index)
{
    product temp;

    temp.name = index.sibling(index.row(), 1).data().toString();
    temp.nomNumber = index.sibling(index.row(), 0).data().toString();
    temp.measure = index.sibling(index.row(), 2).data().toString();
    temp.category = 1;
    temp.count = 0;

    //Открытие окна добавления имущества
    AddItem *addItem = new AddItem(-1, temp,this);
    addItem->setModal(true);
    connect(addItem, SIGNAL(dataToMainWindow(int, product)), SLOT(dataFromAddItem(int, product)));
    addItem->exec();
}

//Поиск в номенклаторе
void MainWindow::on_lineEdit_sprAti_textChanged(const QString &arg1)
{
    if(!baseDB.open()) {
        qDebug() << baseDB.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    QSqlQuery query (baseDB);
    QString text;
    QSqlQueryModel *modelFind = new QSqlQueryModel;
    //Поиск в БД
    text = "SELECT * FROM spr_ati WHERE nomkl_nom like \'%" + arg1.toUpper() + "%\'" + " OR name like \'%" + arg1.toUpper() + "%\' ";
    query.exec(text);
    qDebug() << query.lastError();
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
    baseDB.close();
}

//Выбор определенной группы в номенклаторе
void MainWindow::on_comboBoxGroup_spr_ati_activated(const QString &arg1)
{
    if(!baseDB.open()) {
        qDebug() << baseDB.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    QString text = "SELECT * FROM spr_ati WHERE nomkl_nom like \'17" + arg1.left(3) + "_____\'";
    QSqlQuery query(baseDB);
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
    baseDB.close();
}

//Добавить в распоряжение ВДО безноменклатурное имущество
void MainWindow::on_addBNVDO_clicked()
{
    product temp;

    temp.name = "";
    temp.nomNumber = "";
    temp.measure = "";
    temp.category = 1;
    temp.count = 0;

    //Открытие окна добавления имущества
    AddItem *addItem = new AddItem(-1, temp,this);
    addItem->setModal(true);
    connect(addItem, SIGNAL(dataToMainWindow(int, product)), SLOT(dataFromAddItem(int, product)));
    addItem->exec();
}

//Поиск в лицевом счете при формировании наряда
void MainWindow::on_lineEditFindOrderItem_textChanged(const QString &arg1)
{
    findProductForOrder(arg1); //Функция поиска
    ui->tableWidgetOrderSenderATI->verticalHeader()->hide();
}

void MainWindow::on_tabWidgetOrders_currentChanged(int index)
{
    if (index == 1) ui->tabWidgetRegOrder->setCurrentIndex(0);
    if (index == 2) ui->tabWidgetProvodPrihod_3->setCurrentIndex(0);
}

//Открыть скан распоряжения
void MainWindow::on_openScan_clicked()
{
    openBtnAction();
}

//Загрузить скан распоряжения
void MainWindow::on_downloadScan_clicked()
{
    downloadBtnAction();
}

//Удалить выбранное распоряжение
void MainWindow::on_deleteOrder_clicked()
{
    if(!baseDB.open()) {
        qDebug() << baseDB.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    int row = ui->tableWidgetOrders->currentRow();
    QString queryText = "SELECT id FROM orders WHERE numberOutput = \"" + ui->tableWidgetOrders->item(row, 2)->text() + "\"";
    QSqlQuery query(baseDB);
    query.exec(queryText);
    qDebug() << query.lastError();
    query.next();
    QString id = query.value(0).toString();
    queryText = "SELECT sender FROM orderSenders WHERE id_orders = " + id;
    query.exec(queryText);
    query.next();
    QString vCH = query.value(0).toString();/////////////////////////////////////
    currDB = DataBases[vCH];
    QSqlQuery query2(currDB);
    queryText = "SELECT name, category, count FROM product_orders WHERE id_orders = " + id;
    query.exec(queryText);
    queryText = "SELECT id FROM lic_scheta WHERE title = \"Лицевой счет № 1 (Авиационно-технический склад)\"";
    query2.exec(queryText);
    qDebug() << query2.lastError();
    query2.next();
    QString idLic = query2.value(0).toString();
    while(query.next()) {
        queryText = "SELECT count FROM product_ati WHERE name = \"" + query.value(0).toString() + "\" " + "AND category = \"" + query.value(1).toString() + "\"";
        query2.exec(queryText);
        query2.next();
        qDebug() << query2.lastError();
        double count = query.value(2).toDouble() + query2.value(0).toDouble();
        if(vCH == "в/ч 55443-ТЛ" || vCH == "в/ч 55443-ЛС") {
            queryText = "UPDATE product_ati SET count = " + QString::number(count, 'g', 10) + " WHERE name = \"" + query.value(0).toString() + "\" " + "AND category = \"" + query.value(1).toString() + "\"";
        }else{
            queryText = "UPDATE product_ati SET count = " + QString::number(count, 'g', 10) + " WHERE id_lic_scheta = " + idLic + " AND name = \"" + query.value(0).toString() + "\" " + "AND category = \"" + query.value(1).toString() + "\"";
        }
        query2.exec(queryText);
        qDebug() << query2.lastError();
    }
    queryText = "DELETE FROM product_orders WHERE id_orders = " + id;
    query.exec(queryText);
    queryText = "DELETE FROM orders WHERE id = " + id;
    query.exec(queryText);
    queryText = "DELETE FROM orderSenders WHERE id_orders = " + id;
    query.exec(queryText);
    queryText = "DELETE FROM recipientsOrder WHERE id_orders = " + id;
    query.exec(queryText);

    for(int i = 0; i < orders.size(); ++i) {
        if(orders[i].numberOutput == ui->tableWidgetOrders->item(row, 2)->text()){
            orders.remove(i);
            break;
        }
    }
    ui->tableWidgetOrders->removeRow(row);
    baseDB.close();
}
//Редактировать распоряжение
void MainWindow::on_redactOrder_clicked()
{
    if(!baseDB.open()) {
        qDebug() << baseDB.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    int row = ui->tableWidgetOrders->currentRow();
    QString queryText = "SELECT * FROM orders WHERE numberOutput = \"" + ui->tableWidgetOrders->item(row, 2)->text() + "\"";
    QSqlQuery query(baseDB);
    query.exec(queryText);
    qDebug() << query.lastError();
    query.next();
    QString id = query.value(0).toString();
    order tempOrd;
    tempOrd.name = query.value(1).toString();
    tempOrd.numberOutput = query.value(2).toString();
    tempOrd.dateOutput = query.value(3).toString();
    tempOrd.numberInput = query.value(4).toString();
    tempOrd.dateInput = query.value(5).toString();
    tempOrd.sendType = query.value(8).toString();
    tempOrd.base = query.value(9).toString();
    tempOrd.expirationDate = query.value(10).toString();
    tempOrd.tlg = query.value(11).toString();
    tempOrd.typeProduct = query.value(12).toString();
    tempOrd.completion = query.value(13).toInt();
    tempOrd.target = query.value(14).toString();

    queryText = "SELECT sender FROM orderSenders WHERE id_orders = " + id;
    query.exec(queryText);
    query.next();
    tempOrd.senders.push_back(query.value(0).toString());

    queryText = "SELECT recipient FROM recipientsOrder WHERE id_orders = " + id;
    query.exec(queryText);
    query.next();
    tempOrd.recipients.push_back(query.value(0).toString());/////////////////////////////////////

    currDB = DataBases[tempOrd.senders[0]];
    QSqlQuery query2(currDB);
    queryText = "SELECT * FROM product_orders WHERE id_orders = " + id;
    query.exec(queryText);

    QVector <product> prods;
    while(query.next()) {
        product temp;
        temp.name = query.value(1).toString();
        temp.nomNumber = query.value(2).toString();
        temp.measure = query.value(3).toString();
        temp.count = query.value(4).toDouble();
        temp.factoryNumber = query.value(5).toString();
        temp.category = query.value(6).toInt();
        temp.releaseDate = query.value(7).toString();
        prods.push_back(temp);
    }

    baseDB.close();
    redactFlag = true;

    if(tempOrd.typeProduct == "ВДО") {
        ui->comboBoxTypeOrder->setCurrentIndex(0);
        ui->lineEditOutputOrder->setText(tempOrd.numberOutput);
        ui->dateOutputOrder->setDate(QDate::fromString(tempOrd.dateOutput, "dd.MM.yyyy"));
        ui->lineEditInputOrder->setText(tempOrd.numberInput);
        ui->dateInputOrder->setDate(QDate::fromString(tempOrd.dateInput, "dd.MM.yyyy"));
        ui->dateEditExpiration->setDate(QDate::fromString(tempOrd.expirationDate, "dd.MM.yyyy"));
        ui->orderTargetVDO->setCurrentText(tempOrd.target);
        QString baseDate = tempOrd.base.right(10), baseNumb = tempOrd.base.left(tempOrd.base.size() - 14);
        ui->lineEditBaseOrder->setText(baseNumb);
        ui->dateEdit->setDate(QDate::fromString(baseDate, "dd.MM.yyyy"));
        ui->comboBoxSenderOrder->setCurrentText(tempOrd.senders[0]);
        ui->comboBoxRecipientOrder->setCurrentText(tempOrd.recipients[0]);
        ui->comboBoxSendType->setCurrentText(tempOrd.sendType);
        ui->tabWidgetOrders->setCurrentIndex(1);

        for(int i = 0; i < prods.size(); ++i) {
            dataFromAddItem(-1, prods[i]);
        }
        ui->pushButtonDoneOrder->setText("Сохранить изменения");
    }

    if(tempOrd.typeProduct == "ДО") {
        ui->comboBoxTypeDocPrihod_3->setCurrentIndex(0);
        ui->lineEditOutputNumberOrder->setText(tempOrd.numberOutput);
        ui->dateOutputCreateOrder->setDate(QDate::fromString(tempOrd.dateOutput, "dd.MM.yyyy"));
        //ui->lineEditInputOrder->setText(tempOrd.numberInput);
        //ui->dateInputOrder->setDate(QDate::fromString(tempOrd.dateInput, "dd.MM.yyyy"));
        ui->validOrderDate->setDate(QDate::fromString(tempOrd.expirationDate, "dd.MM.yyyy"));
        ui->orderTarget->setCurrentText(tempOrd.target);
        QString baseDate = tempOrd.base.right(10), baseNumb = tempOrd.base.left(tempOrd.base.size() - 14);
        ui->osnovanie->setText(baseNumb);
        ui->dateEditBase->setDate(QDate::fromString(baseDate, "dd.MM.yyyy"));
        ui->comboBoxOrderSender->setCurrentText(tempOrd.senders[0]);
        on_comboBoxOrderSender_activated(tempOrd.senders[0]);
        ui->comboBoxOrderRecipient->setCurrentText(tempOrd.recipients[0]);
        ui->comboBoxTypeMoveOrder->setCurrentText(tempOrd.sendType);
        ui->tabWidgetOrders->setCurrentIndex(2);

        for(int i = 0; i < prods.size(); ++i) {
            // Добавление имущества в наряд
            int rowIndex = ui->tableWidgetMakeOrder->rowCount();
            ui->tableWidgetMakeOrder->setRowCount(rowIndex + 1);
            ui->tableWidgetMakeOrder->setItem(rowIndex, 0, new QTableWidgetItem(prods[i].name));
            ui->tableWidgetMakeOrder->setItem(rowIndex, 1, new QTableWidgetItem(prods[i].nomNumber));
            ui->tableWidgetMakeOrder->setItem(rowIndex, 2, new QTableWidgetItem(prods[i].measure));
            ui->tableWidgetMakeOrder->setItem(rowIndex, 3, new QTableWidgetItem(QString::number(prods[i].category)));
            ui->tableWidgetMakeOrder->setItem(rowIndex, 4, new QTableWidgetItem(QString::number(prods[i].count)));
            ui->tableWidgetMakeOrder->setItem(rowIndex, 5, new QTableWidgetItem(prods[i].releaseDate));
            ui->tableWidgetMakeOrder->setItem(rowIndex, 6, new QTableWidgetItem(prods[i].factoryNumber));

            //Создание кнопки "Удалить"
            QPushButton *deleteBtn = new QPushButton("Удалить");
            deleteBtn->setFixedSize(128, 30);
            //deleteBtn->setStyleSheet("background-color: rgb(255, 255, 255);");
            connect(deleteBtn, SIGNAL( clicked( bool ) ), SLOT( deleteItemFromTempOrder() ) );

            ui->tableWidgetMakeOrder->setIndexWidget(ui->tableWidgetMakeOrder->model()->index(rowIndex, 8), deleteBtn);

            //connect(addBtn, SIGNAL( clicked( bool ) ), SLOT( addBtnAction() ) );
            //ui->tableWidgetMakeOrder->item(rowIndex, 0)->setTextAlignment(Qt::AlignLeft);
            ui->tableWidgetMakeOrder->item(rowIndex, 1)->setTextAlignment(Qt::AlignCenter);
            ui->tableWidgetMakeOrder->item(rowIndex, 2)->setTextAlignment(Qt::AlignCenter);
            ui->tableWidgetMakeOrder->item(rowIndex, 3)->setTextAlignment(Qt::AlignCenter);
            ui->tableWidgetMakeOrder->item(rowIndex, 4)->setTextAlignment(Qt::AlignCenter);
            ui->tableWidgetMakeOrder->item(rowIndex, 5)->setTextAlignment(Qt::AlignCenter);
            ui->tableWidgetMakeOrder->item(rowIndex, 6)->setTextAlignment(Qt::AlignCenter);
            //ui->tableWidgetMakeOrder->item(rowIndex, 7)->setTextAlignment(Qt::AlignCenter);

            ui->tableWidgetMakeOrder->resizeColumnsToContents();
            //ui->tableWidgetMakeOrder->horizontalHeader()->setStretchLastSection(true);
            ui->tableWidgetMakeOrder->horizontalHeader()->setSectionResizeMode(7, QHeaderView::Stretch);
            ui->tableWidgetMakeOrder->verticalHeader()->hide();
            ui->tableWidgetMakeOrder->setColumnWidth(0, std::max(ui->tableWidgetMakeOrder->columnWidth(0), ui->tableWidgetOrderSenderATI->columnWidth(0)));
        }
        ui->pushButtonDoneMakeOrder->setText("Сохранить изменения");
    }
}


//Нажатие кнопки "Удалить" организацию
void MainWindow::on_buttonDelOrg_2_clicked()
{
    if(!baseDB.open()) {
        qDebug() << baseDB.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    QSqlQuery query(baseDB);
    QString textQuery = "SELECT id FROM organisations WHERE name ='" + ui->tableWidgetOrgs->item(ui->tableWidgetOrgs->currentRow(), 0)->text() + "'";
    query.exec(textQuery);
    qDebug() <<query.lastError();
    query.next();
    int orgID = query.value(0).toInt();
    query.exec("SELECT id FROM departments_of_org WHERE id_organisation = " + QString::number(orgID));
    //Удаление сотрудников
    while(query.next()){
        QSqlQuery delQuery(baseDB);
        delQuery.exec("DELETE FROM humans WHERE id_department = " + query.value(0).toString());
        qDebug() << query.lastError();
    }
    //Удаление отделов
    query.exec("DELETE FROM departments_of_org WHERE id_organisation = " + QString::number(orgID));
    qDebug() << query.lastError();
    //Удаление организации
    query.exec("DELETE FROM organisations WHERE id = " + QString::number(orgID));
    qDebug() << query.lastError();
    importOrgsAndHumans();
    baseDB.close();
}

//Функция получения сведений об организации из базы данных
organisation MainWindow::organisationFromDB(QString name){
    if(!baseDB.open()) {
        qDebug() << baseDB.lastError().text();
    }else{
        qDebug() << "It's OK!";
    }

    QSqlQuery query (baseDB);
    //Считывание данных о выбранной организации
    QString textQuery = "SELECT * FROM organisations WHERE name = '" + name + "'";
    query.exec(textQuery);
    qDebug() << query.lastError();
    query.next();
    int orgID = query.value(0).toInt();

    organisation tempOrg;
    tempOrg.name = query.value(1).toString();
    tempOrg.inn = query.value(2).toString();
    tempOrg.phoneNumber = query.value(3).toString();
    tempOrg.city = query.value(4).toString();
    tempOrg.index = query.value(5).toString();
    tempOrg.vpMORF = query.value(6).toString();
    tempOrg.checkMyOrg = query.value(7).toBool();

    //Считывание данных об отделах (службах) в выбранной организации
    textQuery = "SELECT * FROM departments_of_org WHERE id_organisation = " + QString::number(orgID);
    query.exec(textQuery);
    qDebug() << query.lastError();
    //query.next();
    QVector <depart> tempDeparts;
    while(query.next()) {
        depart tempDepart;
        tempDepart.name = query.value(1).toString();
        int departID = query.value(0).toInt();
        QSqlQuery query2 (baseDB);
        //Считывание данных о сотрудниках выбранного отдела (службы)
        textQuery = "SELECT * FROM humans WHERE id_department = " + QString::number(departID);
        query2.exec(textQuery);
        qDebug() << query2.lastError();
        //query2.next();

        QVector <human> tempHumans;
        while(query2.next()) {
            human tempHuman;
            tempHuman.position = query2.value(1).toString();
            tempHuman.FIO = query2.value(2).toString();
            tempHuman.firstName = query2.value(3).toString();
            tempHuman.secondName = query2.value(4).toString();
            tempHuman.middleName = query2.value(5).toString();
            tempHuman.rank = query2.value(6).toString();
            tempHuman.phoneNumber = query2.value(7).toString();
            tempHuman.email = query2.value(8).toString();

            tempHumans.push_back(tempHuman); //Добавление сотрудника в список
        }
        tempDepart.humanList = tempHumans; //Сохранение списка сотрудников
        tempDeparts.push_back(tempDepart); //Добавление отдела (службы) в список
        tempOrg.departList = tempDeparts; //Сохранение списка отделов (служб)
    }
    baseDB.close();
    return tempOrg;
}

//Нажатие кнопки "Изменить" организацию
void MainWindow::on_buttonChangeOrg_2_clicked()
{
    organisation tempOrg = organisationFromDB(ui->tableWidgetOrgs->item(ui->tableWidgetOrgs->currentRow(), 0)->text());

    //Перенос данных на вкладку информации из временного контейнера
    ui->addNameOrg->setText(tempOrg.name);
    ui->addCityOrg->setText(tempOrg.city);
    ui->addInnOrg->setText(tempOrg.inn);
    ui->addPostcodeOrg->setText(tempOrg.index);
    ui->addPhonenumberOrg->setText(tempOrg.phoneNumber);
    ui->addVpMORFOrg->setText(tempOrg.vpMORF);

    ui->checkBoxMyOrg->setChecked(tempOrg.checkMyOrg);
    ui->comboboxEditDepart->clear();
    for (int i = 0; i < tempOrg.departList.size(); ++i) {
        ui->comboboxEditDepart->addItem(tempOrg.departList[i].name);
    }

    ui->comboboxEditWorker->clear();
    if(!tempOrg.departList.isEmpty()) {
        for (int j = 0; j < tempOrg.departList[0].humanList.size(); ++j) {
            ui->comboboxEditWorker->addItem((tempOrg.departList[0].humanList[j].FIO));
        }
    }
    ui->tabWidgetOrganisations->setCurrentIndex(2);
    ui->tabWidgetOrganisations->setTabText(2, "Редактировать " + tempOrg.name);
}

//Выбор сотрудника при внесении изменений в организацию
void MainWindow::on_comboboxEditWorker_activated(const QString &arg1)
{
    organisation tempOrg = organisationFromDB(ui->addNameOrg->text());

    for(int i = 0; i < tempOrg.departList.size(); ++i) {
        if(tempOrg.departList[i].name == ui->comboboxEditDepart->currentText()) {
            for(int j = 0; j < tempOrg.departList[i].humanList.size(); ++j) {
                if(tempOrg.departList[i].humanList[j].FIO == arg1) {
                    human tempHuman = tempOrg.departList[i].humanList[j];

                    ui->addWorkerPosition->setText(tempHuman.position);
                    ui->addWorkerRank->setText(tempHuman.rank);
                    ui->addWorkerSecName->setText(tempHuman.secondName);
                    ui->addWorkerName->setText(tempHuman.firstName);
                    ui->addWorkerMidName->setText(tempHuman.middleName);
                    ui->addWorkerPhonenumber->setText(tempHuman.phoneNumber);
                    ui->addWorkerEmail->setText(tempHuman.email);
                }
            }
        }
    }

    ui->addWorkerPosition->setReadOnly(false);
    ui->addWorkerPosition->setFocus();

    ui->addWorkerRank->setReadOnly(false);

    ui->addWorkerSecName->setReadOnly(false);

    ui->addWorkerName->setReadOnly(false);

    ui->addWorkerMidName->setReadOnly(false);

    ui->addWorkerPhonenumber->setReadOnly(false);

    ui->addWorkerEmail->setReadOnly(false);
}

//Двойной клик по строчке реестре организаций (открыть подробные сведения об организации)
void MainWindow::on_tableWidgetOrgs_cellDoubleClicked(int row, int column)
{
    organisation tempOrg = organisationFromDB(ui->tableWidgetOrgs->item(row, 0)->text());

    //Перенос данных на вкладку информации из временного контейнера
    ui->nameOrg->setText(tempOrg.name);
    ui->cityOrg->setText(tempOrg.city);
    ui->innOrg->setText(tempOrg.inn);
    ui->postcodeOrg->setText(tempOrg.index);
    ui->phonenumberOrg->setText(tempOrg.phoneNumber);
    ui->vpMORFOrg->setText(tempOrg.vpMORF);

    //Заполнение отделов
    ui->comboboxSelectDepart->clear();
    for (int i = 0; i < tempOrg.departList.size(); ++i) {
        ui->comboboxSelectDepart->addItem(tempOrg.departList[i].name);
    }
    ui->comboboxSelectDepart->setItemText(-1, "Выберите отдел (службу)");
    ui->comboboxSelectDepart->setCurrentIndex(-1);

    //Заполнеие сотрудников
    ui->comboboxSelectWorker->clear();
    if(!tempOrg.departList.isEmpty()) {
        for (int j = 0; j < tempOrg.departList[0].humanList.size(); ++j) {
            ui->comboboxSelectWorker->addItem((tempOrg.departList[0].humanList[j].FIO));
        }
    }
    ui->comboboxSelectWorker->setItemText(-1, "Выберите сотрудника");
    ui->comboboxSelectWorker->setCurrentIndex(-1);

    ui->tabWidgetOrganisations->setCurrentIndex(1);
    ui->tabWidgetOrganisations->setTabText(1, "Подробные данные организации " + tempOrg.name);

    //Очищение полей с данными сотрудника
    ui->workerPosition->clear();
    ui->workerRank->clear();
    ui->workerSecName->clear();
    ui->workerName->clear();
    ui->workerMidName->clear();
    ui->workerPhoneNumber->clear();
    ui->workerEmail->clear();
}

//Выбор сотрудника в КомбоБоксе
void MainWindow::on_comboboxSelectWorker_activated(const QString &arg1)
{
    organisation tempOrg = organisationFromDB(ui->nameOrg->text());

    for(int i = 0; i < tempOrg.departList.size(); ++i) {
        if(tempOrg.departList[i].name == ui->comboboxSelectDepart->currentText()) {
            for(int j = 0; j < tempOrg.departList[i].humanList.size(); ++j) {
                if(tempOrg.departList[i].humanList[j].FIO == arg1) {
                    human tempHuman = tempOrg.departList[i].humanList[j];

                    ui->workerPosition->setText(tempHuman.position);
                    ui->workerRank->setText(tempHuman.rank);
                    ui->workerSecName->setText(tempHuman.secondName);
                    ui->workerName->setText(tempHuman.firstName);
                    ui->workerMidName->setText(tempHuman.middleName);
                    ui->workerPhoneNumber->setText(tempHuman.phoneNumber);
                    ui->workerEmail->setText(tempHuman.email);
                }
            }
        }
    }

    //Запретить редактирование полей
    ui->workerPosition->setReadOnly(true);

    ui->workerRank->setReadOnly(true);

    ui->workerSecName->setReadOnly(true);

    ui->workerName->setReadOnly(true);

    ui->workerMidName->setReadOnly(true);

    ui->workerPhoneNumber->setReadOnly(true);

    ui->workerEmail->setReadOnly(true);
}

//Фильтрация расоряжений по отделам
void MainWindow::on_comboBox_9_activated(const QString &arg1)
{
    if(arg1 == "Все отделы")
        {
            for(int i = 0; i < ui->tableWidgetOrders->rowCount();++i)
            {
                ui->tableWidgetOrders->showRow(i);
            }
        }else
        {
            for(int i = 0; i < ui->tableWidgetOrders->rowCount();++i)
            {
                if(ui->tableWidgetOrders->item(i, 2)->text().left(1) == arg1.left(1))
                {
                    ui->tableWidgetOrders->showRow(i);
                }else
                {
                    ui->tableWidgetOrders->hideRow(i);
                }
            }
        }
}
