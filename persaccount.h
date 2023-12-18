//Класс лицевого счета

#ifndef PERSACCOUNT_H
#define PERSACCOUNT_H
#include "human.h"
#include "product.h"

class PersAccount
{
public:
    PersAccount();
    QString name;
    human mat_otv();
    QVector <product> prodList();
};

#endif // PERSACCOUNT_H
