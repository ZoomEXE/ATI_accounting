#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "entry.h"
#include <QObject>
#include <QMap>
#include <QStandardItem>
//#include <QtSql>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QMap <QString, QWidget*> *hidden_tabs = new QMap <QString, QWidget*>;

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
