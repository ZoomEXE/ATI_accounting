#ifndef TABLEWIDGETITEM_H
#define TABLEWIDGETITEM_H

#include <QTableWidgetItem>
#include <QDate>

class TableWidgetItem : public QTableWidgetItem
{
public:
    explicit TableWidgetItem(const QString &text, int type = Type);
    bool operator< (const QTableWidgetItem&  other) const;

};

#endif // TABLEWIDGETITEM_H
