//Класс организации
#ifndef ORGANISATION_H
#define ORGANISATION_H

#include <QString>
#include <QVector>
#include "human.h"

//class human;
class organisation
{
public:
    organisation();
    QString name; //Наименование организации
    QString index; //Почтовый индекс организации
    QString inn; //ИНН организации
    QString phoneNumber; //Номер телефона
    QString city; //Местоположение организации
    QString vpMORF; //Военное представительство (при наличии)
    bool checkMyOrg; //Флаг - моя организация или нет?
};

#endif // ORGANISATION_H
