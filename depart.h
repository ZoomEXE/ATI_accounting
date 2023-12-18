#ifndef DEPART_H
#define DEPART_H

#include <QVector>
#include <QString>

#include "human.h"

class depart
{
public:
    depart();
    QString name; //Наименование организации
    QVector <human> humanList; // Список сотрудников
};

#endif // DEPART_H
