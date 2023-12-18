#include "scanviewer.h"
#include "ui_scanviewer.h"

scanViewer::scanViewer(QString path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::scanViewer)
{
    ui->setupUi(this);

    pth = path;
    QDir dir(pth);
    scans = dir.entryList();
    QPixmap pix(path + "/" + scans[curScan]);
    ui->scan->setPixmap(pix);
}

scanViewer::~scanViewer()
{
    delete ui;
}

void scanViewer::on_right_clicked()
{
    curScan++;
    if(curScan >= scans.size()) curScan = 2;
    QPixmap pix(pth + "/" + scans[curScan]);
    ui->scan->setPixmap(pix);
}

void scanViewer::on_left_clicked()
{
    curScan--;
    if(curScan < 2) curScan = scans.size() - 1;
    QPixmap pix(pth + "/" + scans[curScan]);
    ui->scan->setPixmap(pix);
}
