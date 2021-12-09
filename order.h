//Класс распоряжения (наряд/разнарядка/план сдачи)
#ifndef ORDER_H
#define ORDER_H
#include <QVector>
#include <QDate>
#include <QImage>

#include "product.h"
#include "human.h"

//class product;
class order
{
public:
    order();
    QString name; //Наименование распоряжения (наряд/разнарядка/план сдачи)
    QVector <product> items; //Вектор имущества на которое выписано распоряжение
    QDate dateInput; //Дата входящего
    QDate dateOutput; //Дата исходящего
    QString numberInput; //Номер входящего
    QString numberOutput; //Номер исходящего
    human chief; //Начальник утвердивший распоряжение
    human oficier; //Офицер выписавший распоряжение
    QString sendType = "средствами получателя"; //Порядок отправки
    QString base; //Основание
    QVector <human> senders; //Отправители
    QVector <human> recipients; //Получатели
    QDate expirationDate; //Годен до
    QString tlg; //Подтверждение телефонограммы
    QString typeProduct; //Тип имущества АТИ/ППТН
    int completion; //Процент выполнения
    QVector <QImage> scans; //Сканы распоряжения
};

#endif // ORDER_H
