#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//Библиотеки для работы с офисом
#include <QAxObject>
#include <QAxWidget>

#include <QMainWindow>
#include <QObject>
#include <QMap>
#include <QStandardItem>
#include <QtSql>
#include <QFileDialog>
//#include <QTableWidget>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include <QCompleter>
#include <cmath>
#include <QList>
#include <QCloseEvent>
#include <QPair>
#include <QAction>

#include "xlsxdocument.h"
#include "product.h"
#include "doc.h"
#include "entry.h"
#include "organisation.h"
#include "additem.h"
#include "additembn.h"
#include "scanviewer.h"
#include "tablewidgetitem.h"
#include "orderdetails.h"
#include "prodinfo.h"
//#include "tablewidgetitem.h"

//#include <QtSql>
//class product;
//class doc;
//class Entry;
//class order;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QMap <QString, QSqlDatabase> DataBases; // Контейнер для хранения баз данных
    QMap <QString, QVector <product>> scheta, dataATI, dataPPTN; //Мап для хранения лицевых счетов с имуществом
    QMap <QString, QVector <product>> orderDataATI;//Временный контейнер для хранения имущества из БД (для нарядов)
    QMap <QString, QWidget*>* hidden_tabs = new QMap <QString, QWidget*>; //Мап для хранения вкладок
    QVector <order> orders; //Вектор для хранения нарядов
    int currentScanIndex = 0; //Индекс текущего скана
    QVector <doc> inputDocs, outputDocs; //Вектор для хранения приходных и расходных документов
    QVector <QImage> tempScans; //Вектор для хранения сканов
    order tempOrder; //Временное хранилище данных регистрируемого наряда
    QMap <QString, QMap <QString, QVector <human> > > organisations; //Мап организаций с подразделениями и векторами их сотрудников
    QMap <QString, QVector <human> > tempDeps; //Временное хранилище подразделений и сотрудников
    QMap <QString, organisation> orgsContainer; //Контейнер для хранения данных об организациях
    QSqlDatabase currDB, senderDB, baseDB; //Текущая база данных / База данных из которой отправляется имущество (для нарядов) / Основная база данных
    QVector <product> tempOrderProducts; //Временное хранилище имущества, на которое выписывается наряд
    QString currUser = "";// Текущий пользователь
    QString currUserRights = ""; // Права текущего пользователя

    doc tempDoc; //Временное хранилище проводимого/формируемого документа
    QVector <product> tempDocProducts; //Временное хранилище имущества в проводимом/формируемом документе
    QVector <doc> docs; //Хранилище проведенных документов
    QVector <product> sprATI; //Хранилище для справочника АТИ
    bool redactFlag = false; // Флаг: true - распоряжение редактируется(удаляем старое, сохраняем новое), false - распоряжение новое (просто сохраняем)

private slots:

    void showContextMenuOrders(const QPoint &pos);
    void openOrderDetails();
    void dataFromAddItem(int row, product item); //Слот получения данных из окна добавления имущества
    //void dataFromAddItemBN(int row, product item); //Слот получения данных из окна добавления имущества Б/Н
    void on_log_out_triggered();
    void close_entry(QString user, QString rights);
    void on_tabWidget_tabCloseRequested(int index); //Закрытие вкладки
    void new_tab(QString name); //Открытие новой вкладки
    bool check_available_tab(QString name); //Проверка на наличие открытой вкладки
    void on_spr_ati_triggered(); //Открытие справочника АТИ

    void on_spr_pptn_2_triggered();  //Открытие справочника ППТН

    void on_info_triggered(); // Открытие справки

    void on_edit_profile_data_triggered();

    void on_change_pass_triggered();

    void on_action_users_edit_triggered();

    void on_action_podr_edit_triggered();

    void on_action_sklad_triggered(); //Открытие учета в воинской части

    void on_action_remont_triggered();

    void on_action_prihod_triggered();

    void on_action_rashod_triggered();

    void save_first_tabs();

    //void on_comboBoxFindtypeAti_activated(int index);

    //void on_findInAtiButton_clicked();

    //void on_findGroup_inAtiButton_clicked();

    void on_findInPptnButton_clicked();

    void on_comboBoxfindTypePptn_activated(int index);

    void slotModelItemChanged(QStandardItem *item);
    void on_import_Excel_triggered();

    void on_importExcelpushButton_clicked();

    void on_comboBox_2_activated(const QString &arg1);

    void on_pushButton_3_clicked();

    void add_lic_schet(QString arg);
    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_comboBoxScheta_activated(const QString &arg1);

    void on_checkBoxAllDocumentsPrihod_stateChanged(int arg1);

    void on_checkBoxAllDocumentsRashod_stateChanged(int arg1);

    //void on_orders_triggered();

    void importBD();
    void on_pushButtonNextPrihod_2_clicked();

    //void on_pushButtonScanOrder_clicked();
    void add_scans();

    //void on_pushButtonRightOrder_clicked();

    //void on_pushButtonLeftOrder_clicked();

    void on_organisations_triggered();

    void on_buttonAddWorker_clicked();

    void on_buttonAddDepart_clicked();

    void on_buttonRemoveDepart_clicked();

    void on_buttonResetWorker_clicked();

    void on_buttonSaveWorker_clicked();

    void on_comboboxEditDepart_activated(const QString &arg1);

    void on_buttonRemoveWorker_clicked();

    void on_comboboxEditWorker_activated(int index);

    void on_buttonSaveOrg_clicked();

    void on_buttonCancelAddOrg_clicked();

    void importOrgsAndHumans(); //Функция считывания из БД информации об организациях, подразделениях и сотрудниках

    void on_infoAboutOrg_triggered();

    void on_comboboxSelectDepart_activated(const QString &arg1);

    void on_comboboxSelectWorker_activated(int index);

    void on_buttonChangeOrg_clicked();

    //void on_pushButtonNextOrder_clicked();

    void on_pushButtonCancelOrder_clicked();

    //void on_pushButtonCancelOderMC_clicked();

    void on_pushButtonDoneOrder_clicked();

    void on_checkBoxAllDocumentsPrihod_2_stateChanged(int arg1);

    //void on_pushButtonFindPrihod_2_clicked();

    void on_importExcelpushButton_2_clicked();

    void on_connectBD_triggered();

    void on_tableWidgetConnectedBD_cellActivated(int row, int column);

    void on_useBD_clicked();

    void on_tableWidgetConnectedBD_cellClicked(int row, int column);

    void on_comboBoxOrderSender_activated(const QString &arg1);

    void on_comboBoxShetaOfSender_activated(const QString &arg1);

    void findProductForOrder(QString name);

    void on_pushButton_clicked();

    void on_pushButtonDoneMakeOrder_clicked();

    void on_tableWidgetOrderSenderATI_cellDoubleClicked(int row, int column);

    void on_pushButtonFindPrihod_3_clicked();

    void connectedDB(QStringList dbPaths); //Подключение баз данных

    bool findSubStr(QString search, QString text); //Функция поиска подстроки в строке

    void addOrderToDB(order tempOrder); //Функция добавления распоряжения в базу данных
    void on_selectVCh_activated(const QString &arg1);

    void on_findInAtiInSkladButton_clicked();

    void on_comboBoxFindtypeAti_activated(const QString &arg1);

    void on_lineEditOutputNumberOrder_textChanged(const QString &arg1);

    void ordersFromBD(); // Забираем наряды из БД

    void docsFromBD(); // Забираем документы из БД

    //void on_tableWidgetOrderSenderATI_cellActivated(int row, int column);

    void addBtnAction(); // Действие по нажатию кнопки "Добавить"

    void on_pushButtonCancelMakeOrder_clicked();

    void deleteItemFromTempOrder(); // Действие по нажатию кнопки "Удалить"

    void deleteItemFromTempOrderVDO(); // Действие по нажатию кнопки "Удалить" (Распоряжения ВДО)

    void on_comboBoxOrderRecipient_activated(const QString &arg1);

    void on_dateEditBeginOrder_userDateChanged(const QDate &date);

    void on_dateEditEndOrder_userDateChanged(const QDate &date);

    void dateSort (const QDate &dateBegin, const QDate &dateEnd); // Отбор нарядов по дате

    void on_comboBoxSenders_2_activated(const QString &arg1);

    void on_comboBoxTypeMovePrihod_2_activated(const QString &arg1);

    void on_comboBoxTypeProductPrihod_2_activated(const QString &arg1);

    void downloadBtnAction(); //Действия по нажатию кнопки "Загрузить скан"

    void openBtnAction(); //Действия по нажатию кнопки "Открыть скан"

    //void on_pushButtonScanRashod_clicked();

    //void on_pushButtonScanPrihod_clicked();

    void on_pushButton_5_clicked();

    void on_pushButtonRightPrihod_clicked();

    void on_tableWidgetOrders_cellDoubleClicked(int row, int column);

    void on_lineEditFindPrihod_2_textChanged(const QString &arg1);

    void on_pushButtonAddLinePrihod_clicked();

    void on_pushButtonAddLineRashod_clicked();

    void on_pushButtonRemoveLineRashod_clicked();

    void on_pushButtonRemoveLinePrihod_clicked();

    void on_comboBoxTypeMoveProvodPrihod_activated(int index);

    void on_pushButtonNextPrihod_clicked();

    void on_pushButtonBackPrihod_clicked();

    void on_pushButtonDonePrihod_clicked();

    void on_lineEdit_3_textChanged(const QString &arg1);

    void on_tableWidgetOrderNomkl_cellDoubleClicked(int row, int column);

    void on_tableViewNomklVDO_doubleClicked(const QModelIndex &index);

    void on_lineEdit_sprAti_textChanged(const QString &arg1);

    void on_comboBoxGroup_spr_ati_activated(const QString &arg1);

    void on_addBNVDO_clicked();

    void on_lineEditFindOrderItem_textChanged(const QString &arg1);

    void on_tabWidgetOrders_currentChanged(int index);

    void on_openScan_clicked();

    void on_downloadScan_clicked();

    void on_deleteOrder_clicked();

    void on_redactOrder_clicked();

    //void on_comboboxSelectOrg_activated(const QString &arg1);

    //void on_comboboxSelectOrg_activated(int index);

    void on_buttonDelOrg_2_clicked();

    void on_buttonChangeOrg_2_clicked();

    organisation organisationFromDB(QString name); //Получение сведений об организации из базы данных

    void on_comboboxEditWorker_activated(const QString &arg1);

    void on_tableWidgetOrgs_cellDoubleClicked(int row, int column);

    void on_comboboxSelectWorker_activated(const QString &arg1);

    void on_comboBox_9_activated(const QString &arg1);

    void on_tableAccount_doubleClicked(const QModelIndex &index);

    void on_lineEditFindInSklad_textChanged(const QString &arg1);

    void on_lineEditFindInSklad_textEdited(const QString &arg1);

    void on_action_triggered();

    void on_rasporReestr_triggered();

    void on_regRaspr_triggered();

    void on_formRaspr_triggered();

private:
    Ui::MainWindow *ui;

protected:
    QStandardItemModel* model;
    QStandardItem* categ_1;
    QStandardItem* categ_2;
    QStandardItem* categ_3;
    QStandardItem* categ_4;
    QStandardItem* categ_5;
};

#endif // MAINWINDOW_H
