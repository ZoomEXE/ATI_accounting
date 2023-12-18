//Класс распоряжения (наряд/разнарядка/план сдачи)
#ifndef ORDER_H
#define ORDER_H
#include <QVector>
#include <QDate>
#include <QImage>

#include "product.h"
#include "human.h"
#include "organisation.h"
#include "doc.h"

//class product;
class order
{
public:
    order();
    QString name; //Наименование распоряжения (наряд/разнарядка/план сдачи)
    QVector <product> items; //Вектор имущества на которое выписано распоряжение
    QString dateInput; //Дата входящего
    QString dateOutput; //Дата исходящего
    QString numberInput; //Номер входящего
    QString numberOutput; //Номер исходящего
    QString chief; //Начальник утвердивший распоряжение
    QString oficier; //Офицер выписавший распоряжение
    QString sendType = "средствами получателя"; //Порядок отправки
    QString base; //Основание
    QVector <QString> senders; //Отправители
    QVector <QString> recipients; //Получатели
    QString expirationDate; //Годен до
    QString tlg; //Подтверждение телефонограммы
    QString typeProduct; //Тип имущества АТИ/ППТН
    int completion; //Процент выполнения
    QVector <QImage> scans; //Сканы распоряжения
    QString target; //Цель операции
    QVector <doc> orderDocs; //Первичные учетные документы на основании данного распоряжения
    void clear(); //Очистить переменную
};

#endif // ORDER_H
