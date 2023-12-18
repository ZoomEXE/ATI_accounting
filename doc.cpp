#include "doc.h"
//#include "product.h"
//#include "order.h"
doc::doc()
{

}

void doc::clear(){
    name.clear(); //Наименование
    numberInput.clear(); //Номер входящего
    numberOutput.clear(); //Номер исходящего
    commanderPosition.clear(); //Должность командира, утверждающего документ
    commanderName.clear(); //ФИО командира
    base.clear(); //Основание
    items.clear(); //Вектор имущества входящего в документ
    senderOrg.clear(); //Организация отправитель
    senderDepart.clear(); //Стуктурное подразделение отправителя
    recipientOrg.clear(); //Организация получатель
    recipientDepart.clear(); //Структурное подразделение получателя
    executorPosition.clear(); //Должность исполнителя документа
    executorName.clear(); //ФИО исполнителя документа
    bookerName.clear(); // ФИО бухгалтера
    senderPosition.clear(); //Должность отпустившего имущество
    senderName.clear(); //ФИО отпустившего имущество
    recipientPosition.clear(); //Должность получившего имущество
    recipientName.clear(); //ФИО получившего имущество
    baseOrder.clear(); //Распоряжение на основании которого выписан первичный документ

}
