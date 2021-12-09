#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QMap>
#include <QStandardItem>
#include <QtSql>
#include <QFileDialog>

#include "xlsxdocument.h"
#include "product.h"
#include "doc.h"
#include "entry.h"
#include "organisation.h"

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
    QMap <QString, QVector <product>> scheta, dataATI, dataPPTN; //Мап для хранения лицевых счетов с имуществом
    QMap <QString, QWidget*>* hidden_tabs = new QMap <QString, QWidget*>; //Мап для хранения вкладок
    QVector <order> orders; //Вектор для хранения нарядов
    QVector <doc> inputDocs, outputDocs; //Вектор для хранения приходных и расходных документов
    QVector <QImage> tempScans; //Вектор для хранения сканов
    int currentScanIndex = 0; //Индекс текущего отображаемого скана

    QMap <QString, QMap <QString, QVector <human> > > organisations; //Мап организаций с подразделениями и векторами их сотрудников
    QMap <QString, QVector <human> > tempDeps; //Временное хранилище подразделений и сотрудников
    QMap <QString, organisation> orgsContainer; //Контейнер для хранения данных об организациях

private slots:

    void on_log_out_triggered();
    void close_entry();
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

    void on_comboBoxFindtypeAti_activated(int index);

    void on_findInAtiButton_clicked();

    void on_findGroup_inAtiButton_clicked();

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

    void on_orders_triggered();

    void importBD();
    void on_pushButtonNextPrihod_2_clicked();

    void on_pushButtonScanOrder_clicked();
    void add_scans();

    void on_pushButtonRightOrder_clicked();

    void on_pushButtonLeftOrder_clicked();

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
