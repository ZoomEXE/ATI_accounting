//Класс приходных/расходных документов
#ifndef DOC_H
#define DOC_H
//#include "product.h"
//#include "order.h"
#include <QString>
#include <QDate>

class product;
class order;
class doc
{
public:
    doc();
    QString name; //Наименование документа
    QDate dateInput; //Дата входящего
    QString numberInput; //Номер входящего
    QDate dateOutput; //Дата исходящего
    QString numberOutput; //Номер исходящего
    QString commanderPosition; //Должность командира, утверждающего документ
    QString commanderName; //ФИО командира
    QString base; //Основание
    QVector <product> *items; //Вектор имущества входящего в документ
    QString senderOrg; //Организация отправитель
    QString senderDepart; //Стуктурное подразделение отправителя
    QString recipientOrg; //Организация получатель
    QString recipientDepart; //Структурное подразделение получателя
    QString executorPosition; //Должность исполнителя документа
    QString executorName; //ФИО исполнителя документа
    QString bookerName; // ФИО бухгалтера
    QString senderPosition; //Должность отпустившего имущество
    QString senderName; //ФИО отпустившего имущество
    QString recipientPosition; //Должность получившего имущество
    QString recipientName; //ФИО получившего имущество
    order* baseOrder; //Распоряжение на основании которого выписан первичный документ
};

#endif // DOC_H
