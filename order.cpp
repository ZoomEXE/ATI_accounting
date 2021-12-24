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
    order::dateInput.setDate(2000,1,1);
    order::completion = 0;
    order::dateOutput.setDate(2000,1,1);
    order::numberInput = "";
    order::typeProduct = "";
    order::numberOutput = "";
    order::expirationDate.setDate(2000,1,1);
}
