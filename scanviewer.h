#ifndef SCANVIEWER_H
#define SCANVIEWER_H

//#include <QWidget>
#include <QDialog>
#include <QDir>
namespace Ui {
class scanViewer;
}

class scanViewer : public QDialog
{
    Q_OBJECT

public:
    explicit scanViewer(QString path, QWidget *parent = nullptr);
    ~scanViewer();

    QStringList scans;
    int curScan = 2;
    QString pth;
private slots:
    void on_right_clicked();

    void on_left_clicked();

private:
    Ui::scanViewer *ui;
};

#endif // SCANVIEWER_H
