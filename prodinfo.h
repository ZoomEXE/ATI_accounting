#ifndef PRODINFO_H
#define PRODINFO_H

#include <QDialog>
#include <QMap>

#include <doc.h>
#include <product.h>

namespace Ui {
class ProdInfo;
}

class ProdInfo : public QDialog
{
    Q_OBJECT

public:
    explicit ProdInfo(QVector <product> tempProds, QVector <doc> tempDocs, QWidget *parent = nullptr);
    ~ProdInfo();

private slots:
    void on_mol_activated(int index);
    int findColumn (QString mol); //Функция поиска колонки с определенным МОЛ

    void on_mol_activated(const QString &arg1);

private:
    Ui::ProdInfo *ui;
};

#endif // PRODINFO_H
