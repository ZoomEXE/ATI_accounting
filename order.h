//Класс распоряжения (наряд/разнарядка/план сдачи)
#ifndef ORDER_H
#define ORDER_H
#include <QVector>
#include <QDate>

class product;
class order
{
public:
    order();
    QString name; //Наименование распоряжения (наряд/разнарядка/план сдачи)
    QVector <product>* items; //Вектор имущества на которое выписано распоряжение
    QDate dateInput; //Дата входящего
    QDate dateOutput; //Дата исходящего
    QString numberInput; //Номер входящего
    QString numberOutput; //Номер исходящего
    QString chiefName; //ФИО начальника утвердившего распоряжение
    QString chiefRank; //Звание начальника -//-
    QString chiefPosition; //Должность начальника -//-
    QString oficierName; //ФИО офицера выписавшего распоряжение
    QString oficierRank; //Звание офицера -//-
    QString oficierPosition; //Должность офицера -//-
    QString sendType = "средствами получателя"; //Порядок отправки
    QString base; //Основание
    QVector <QString> senders; //Отправители
    QVector <QString> recipients; //Получатели
    QDate expirationDate; //Годен до
    QString tlg; //Подтверждение телефонограммы
    QString typeProduct; //Тип имущества АТИ/ППТН
    int completion; //Процент выполнения
};

#endif // ORDER_H
