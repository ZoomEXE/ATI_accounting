#include "tablewidgetitem.h"

TableWidgetItem::TableWidgetItem(const QString &text, int type) :
    QTableWidgetItem(text,type)
{
}

bool TableWidgetItem::operator <(const QTableWidgetItem&  other) const
{
    return (other.data( Qt::DisplayRole ) > other.data(Qt::DisplayRole ));
}
