#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <entry.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    Entry *ent = new Entry;
    QObject::connect(ent, SIGNAL(close_signal()), this, SLOT(close_entry()));
    ent->setModal(true);
    ent->exec();
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_log_out_triggered()
{
    Entry *ent = new Entry;
    QObject::connect(ent, SIGNAL(close_signal()), this, SLOT(close_entry()));
    ent->setModal(true);
    ent->exec();
}

void MainWindow::close_entry() {
    MainWindow::close();
}
