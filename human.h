//Класс должностного лица
#ifndef HUMAN_H
#define HUMAN_H
#include <QString>

class human
{
public:
    human();
    QString position; //Должность
    QString FIO; // ФИО образца Иванов И.И.
    QString firstName; //Имя
    QString secondName; //Фамилия
    QString middleName; //Отчество
    QString rank; //Воинское звание при наличии
    QString phoneNumber; // Номер телефона
    QString email; // Электронная почта
};

#endif // HUMAN_H
