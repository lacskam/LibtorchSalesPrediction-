#include "mslq.h"
#include "plot.h"
#include <QApplication>

DB dbase;

int main(int argc, char *argv[])
{

    int df =8;
    QList<QDate> d = {QDate(2019,1,1),QDate(2022,3,8)};

    QList<Data> data = getDateFromDb(&dbase);

    QApplication a(argc, argv);

        Plot myPlot = Plot(0,&data,&df,&d,&dbase);

        myPlot.show();

        return a.exec();

}
