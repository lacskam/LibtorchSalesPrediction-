#ifndef PLOT_H
#define PLOT_H
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QLabel>
#include<QPushButton>
#include<QDateEdit>
#include <QFileDialog>
#include<QLineEdit>
#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QtCharts>
#include "mslq.h"
QT_CHARTS_USE_NAMESPACE

class Plot : public QWidget {
    Q_OBJECT
public:
    Plot(QWidget *parent = 0,QList<Data> *dataFromDb= nullptr,qint32 *pickedprod = nullptr, QList<QDate> *endDAte= nullptr,DB *dbase = nullptr);
public slots:
    void update(QList<Data> *dataFromDb, DB *dbase1, QValueAxis *axisY, QDateTimeAxis *axisX, QSpinBox *lePickedProd, QDateEdit *leFirstDate, QDateEdit *leSecondDate,QMap<QDateTime,qint32> *mapSaless);
    void getfile(QSpinBox *lePickedProd,QMap<QDateTime,qint32> *mapSales);
    void openfile();


private:

    QMap<QDateTime,qint32> *mapSaless;

    QWidget *main;
    QChart *chart;
    QChartView *chartView;
    QHBoxLayout *hb;
    QVBoxLayout *vb;
    QDateEdit *leFirstDate;
    QDateEdit *leSecondDate;
    QSpinBox *lePickedProd;
    QPushButton *btn;

    QPushButton *btnGetFile;
    QPushButton *btnOpenFile;
};

#endif // PLOT_H
