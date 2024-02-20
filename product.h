//Класс имущества (АТИ/ППТН)
#ifndef PRODUCT_H
#define PRODUCT_H
#include <QString>
#include <QDate>
//#include <doc.h>

//#include "xlsxdocument.h"

//class doc;
class product
{
public:
    product();
    QString nomNumber; //Номенклатурный номер имущества
    QString name; //Наименование имущества
    QString measure; //Единица измерения имущества
    double count; //Количество имущества
    double price = 0; // Цена имущества
    QString factoryNumber = "б/н"; //Заводской номер имущества
    int category; //Категория имущества
    QString releaseDate; //Дата изготовления имущества
    bool ordered = false; //Флаг: true - на данное имущество выписан наряд/разнарядка/план сдачи, false - на данное имущество не выписан наряд/разнарядка/план сдачи
    QString prihodDoc; //Приходный документ, на основании которого поступило имущество
    QString importDoc; //Путь до файла из которого было импортировано имущество (если было импортировано)
    QString orderNumber; //Распоряжение выписанное на это имущество
    QString type117; // Тип АТИ согласно приказа ГК ВКС № 117
};

#endif // PRODUCT_H
