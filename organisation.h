//Класс организации
#ifndef ORGANISATION_H
#define ORGANISATION_H

#include <QString>
#include <QVector>
#include "human.h"

class organisation
{
public:
    organisation();
    QString name; //Наименование организации
    int index; //Почтовый индекс организации
    QVector <QString> departments; //Подразделения в организации
    int inn; //ИНН организации
    human comannder; //"Командир" организации
    QString phoneNumber; //Номер телефона
    QString city; //Местоположение организации
    QString vpMORF; //Военное представительство (при наличии)

};

#endif // ORGANISATION_H
