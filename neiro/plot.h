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
#include <QPainter>
#include <QLabel>


QT_CHARTS_USE_NAMESPACE

class Plot : public QWidget {
    Q_OBJECT
public:
    Plot(QWidget *parent = 0,QList<Data> *dataFromDb= nullptr,qint32 *pickedprod = nullptr, QList<QDate> *endDAte= nullptr,DB *dbase = nullptr);
public slots:
    void updateChart(QList<Data> *dataFromDb, DB *dbase1, QValueAxis *axisY, QDateTimeAxis *axisX,
                     QSpinBox *lePickedProd, QDateEdit *leFirstDate, QDateEdit *leSecondDate,QMap<QDateTime,qint32> *mapSaless);
    void getfile(QSpinBox *lePickedProd,QMap<QDateTime,qint32> *mapSales);
    void openfile(QValueAxis *axisY, QDateTimeAxis *axisX);
    void updateNN(DB *dbase1, QValueAxis *axisY, QDateTimeAxis *axisX,
                  QSpinBox *lePickedProd, QDateEdit *leFirstDate, QDateEdit *leSecondDate);
    void lineMove(QMap<QDateTime,qint32> *mapSaless);
    void lineMoveNN(QMap<QDateTime,float> *mapSaless);

private:

    QMap<QDateTime,qint32> *mapSaless;
    QMap<QDateTime,float> *mapNN;

    QWidget *main;
    QChart *chart;
     QChart *chartnn;
    QHBoxLayout *hblcharts;
    QPushButton *plotNNButton;
    QChartView *chartView;
    QChartView *chartViewNN;
    QSplineSeries *ser;
    QSplineSeries *serNN;

    QHBoxLayout *hb;
    QVBoxLayout *vb;
    QDateEdit *leFirstDate;
    QDateEdit *leSecondDate;
    QSpinBox *lePickedProd;
    QPushButton *btn;

    QPushButton *btnGetFile;
    QPushButton *btnOpenFile;

    QSlider *sliderBase;
    QSlider *sliderNN;
    int *slider_range;
    int *slider_rangeNN;
    QHBoxLayout *vbs;
    QGraphicsLineItem *line;
    QGraphicsLineItem *lineNN;
    int x1;
    int y1;
    int x2;
    int y2;
    QLabel *labelValue;
    QLabel *labelValueNN;

};

#endif // PLOT_H
