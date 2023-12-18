#include "order.h"
#include <product.h>

order::order()
{

}

//Функция очистки переменной
void order::clear()
{
    order::chief = "";
    order::tlg = "";
    order::base = "";
    order::name = "";
    order::items.clear();
    order::scans.clear();
    order::oficier = "";
    order::senders.clear();
    order::recipients.clear();
    order::sendType = "";
    order::dateInput = "";
    order::completion = 0;
    order::dateOutput = "";
    order::numberInput = "";
    order::typeProduct = "";
    order::numberOutput = "";
    order::expirationDate = "";
}
