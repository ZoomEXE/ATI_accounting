#include "mainwindow.h"
#include <QApplication>
#include <entry.h>

#include "header/xlsxdocument.h"
#include "header/xlsxchartsheet.h"
#include "header/xlsxcellrange.h"
#include "header/xlsxchart.h"
#include "header/xlsxrichstring.h"
#include "header/xlsxworkbook.h"
using namespace QXlsx;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
