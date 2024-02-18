#include "plot.h"
#include <algorithm>
#include <QWidget>

QList<QPointF> points;
Plot::Plot(QWidget *parent, QList<Data> *dataFromDb, qint32 *pickedprod, QList<QDate> *endDAte, DB *dbase1) : QWidget(parent) {
    vb = new QVBoxLayout(this);
    hb = new QHBoxLayout;
    hblcharts = new QHBoxLayout;
    vbs = new QHBoxLayout;


    vb->addLayout(hblcharts);

    QDateEdit *leFirstDate = new QDateEdit;

    hb->addWidget(leFirstDate);

    leSecondDate = new QDateEdit;

    hb->addWidget(leSecondDate);

    lePickedProd = new QSpinBox;
    lePickedProd->setRange(0, 153);

    hb->addWidget(lePickedProd);

    QValueAxis *axisY = new QValueAxis();
    QDateTimeAxis *axisX = new QDateTimeAxis();

    QValueAxis *axisYnn = new QValueAxis();
    QDateTimeAxis *axisXnn = new QDateTimeAxis();



    QMap<QDateTime,qint32> *mapSaless = new QMap<QDateTime,qint32>;

    btn = new QPushButton("Plot");
    connect(btn, &QPushButton::clicked, this, [=]() {
        updateChart(dataFromDb, dbase1, axisY, axisX, lePickedProd, leFirstDate, leSecondDate,mapSaless);
    });
    plotNNButton = new QPushButton("PlotNN");
    connect(plotNNButton, &QPushButton::clicked, this, [=]() {
        updateNN(dbase1, axisYnn, axisXnn, lePickedProd, leFirstDate, leSecondDate);
    });

    hb->addWidget(btn);
    hb->addWidget(plotNNButton);

    btnGetFile = new QPushButton("Get flie");
    connect(btnGetFile, &QPushButton::clicked, this, [=]() {
       getfile(lePickedProd,mapSaless);
    });

    btnOpenFile = new QPushButton("Open flie");
    connect(btnOpenFile, &QPushButton::clicked, this, [=]() {
       openfile(axisY, axisX);
    });



     hb->addWidget(btnGetFile);
     hb->addWidget(btnOpenFile);


    chart = new QChart();
    chartnn = new QChart();


    QList<QDateTime> dates;
    QList<qint32> values;

    *mapSaless = (getNumSalesProd(pickedprod,dataFromDb,endDAte));

    for (QMap<QDateTime,qint32>::const_iterator i=mapSaless->begin();i!=mapSaless->end();i++) {
        dates.push_back(i.key());
        values.push_back(i.value());
    }


   /*for (int i = 0; i < dataFromDb->size(); i++) {
        if (dateCheker(&(dataFromDb->at(i).date), endDAte) && !dates.contains(QDateTime(dataFromDb->at(i).date))) {
            dates.append(QDateTime(dataFromDb->at(i).date));
            values.append(std::count(dataFromDb->at(i).prod.begin(), dataFromDb->at(i).prod.end(), *pickedprod));
        }
    }*/

    ser = new QSplineSeries();
    serNN = new QSplineSeries();


    for (int i = 0; i < dates.size(); ++i) {
        ser->append(dates[i].toMSecsSinceEpoch(), values[i]);
    }

    axisX->setTickCount(11);
    axisX->setFormat("dd.MM.yyyy");

    line = new QGraphicsLineItem(0, 0, 0, 0);
    line->setPen(QPen(Qt::red,1));

    lineNN = new QGraphicsLineItem(0, 0, 0, 0);
    lineNN->setPen(QPen(Qt::red,1));

    axisXnn->setTickCount(11);
    axisXnn->setFormat("dd.MM.yyyy");

    chart->addSeries(ser);
    chart->addAxis(axisX, Qt::AlignBottom);

    chartnn->addSeries(serNN);
    chartnn->addAxis(axisXnn, Qt::AlignBottom);

    ser->attachAxis(axisX);
    serNN->attachAxis(axisXnn);

    chart->addAxis(axisY, Qt::AlignLeft);
    chartnn->addAxis(axisYnn, Qt::AlignLeft);

    ser->attachAxis(axisY);
      serNN->attachAxis(axisYnn);

    axisX->setTitleText("дата");
    axisY->setTitleText("продажи \"" + dbase1->getProductName(pickedprod) + "\"");

    axisXnn->setTitleText("дата");
    axisYnn->setTitleText("продажи \"" + dbase1->getProductName(pickedprod) + "\"");

    chartView = new QChartView(this->chart);
     chartViewNN = new QChartView(this->chartnn);


     sliderBase = new QSlider(Qt::Horizontal,chartView);
     sliderBase->setStyleSheet("QSlider::groove:horizontal { border: none; background: none; }"
                                  "QSlider::handle:horizontal { background: #024fad; width: 10px; height: 5px; border-radius: 5px; }");

    sliderBase->resize(width()*1.27,15);
     sliderBase->move(width()/8.2,height()/7.5);
      connect(sliderBase,&QSlider::valueChanged,this,[=]() {
          lineMove(mapSaless);
      });

     sliderNN = new QSlider(Qt::Horizontal,chartViewNN);
     sliderNN->setStyleSheet("QSlider::groove:horizontal { border: none; background: none; }"
                                  "QSlider::handle:horizontal { background: #024fad; width: 10px; height: 5px; border-radius: 5px; }");
     connect(sliderNN,&QSlider::valueChanged,this,[=]() {
         lineMoveNN(mapNN);
     });
     sliderNN->resize(width()*1.26,15);
      sliderNN->move(width()/6.8,height()/7.5);

    slider_range = new int(1);
    slider_rangeNN = new int(1);
    labelValue = new QLabel(chartView);
    labelValueNN = new QLabel(chartViewNN);

    labelValueNN->move(width()*2,height()*2);
    labelValue->move(width()*2,height()*2);

    labelValue->setText("<p style=\"color: rgb(0, 0, 0)\">34365</p>");
    labelValueNN->setText("<p style=\"color: rgb(0, 0, 0)\">34365</p>");

    hblcharts->addWidget(chartView);
    hblcharts->addWidget(chartViewNN);

    vb->addLayout(hb);

    chart->scene()->addItem(line);
    chartnn->scene()->addItem(lineNN);

}


void Plot::openfile(QValueAxis *axisY, QDateTimeAxis *axisX) {
    QString filePath = QFileDialog::getOpenFileName(nullptr, "Выберите файл", "", "Файлы с расширением .sex (*.sex);;Все файлы (*.*)");
    int status =0;
    QString fileCont;
    if (!filePath.isEmpty()) {

        QFile file(filePath);

        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QTextStream stream(&file);


            fileCont = stream.readAll();

            status =1;
            file.close();
        } else {
            qDebug() << "Ошибка открытия файла для чтения";
        }
        }
     else  qDebug() << "Отменено пользователем";


    if (status ==1 && !fileCont.isEmpty()) {
        QList<QDateTime> dates;
        QList<qint32> values;
        QList<QString> list;
        ser = new QSplineSeries();
        list = fileCont.split('\n');
        for (int i=0;i<list.size()-1;i++) {
            dates.push_back(QDateTime(QDate(list[i].split(" ")[1].split(".")[0].toInt(),list[i].split(" ")[1].split(".")[1].toInt(),list[i].split(" ")[1].split(".")[2].toInt())));
            qDebug()<<dates[i];
            values.push_back(std::abs(list[i].split(" ")[0].toFloat()));
        }
    \

        chart->removeAllSeries();
        chart->series().clear();

        for (int i = 0; i < dates.size(); ++i) {
            ser->append(dates[i].toMSecsSinceEpoch(), values[i]);

        }

        chart->addSeries(ser);

        axisX->setRange(dates.first(), dates.last());
                axisY->setRange(*std::min_element(values.begin(), values.end()), *std::max_element(values.begin(), values.end()));



    }
}


void Plot::getfile(QSpinBox *lePickedProd,QMap<QDateTime,qint32> *mapSales) {
    QFile file(QString::number(lePickedProd->value())+"_prod.sex");

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {

           QTextStream stream(&file);
           for (QMap<QDateTime,qint32>::const_iterator i=mapSales->begin();i!=mapSales->end();i++) {
               stream << QString::number(i.value()) + " " + i.key().date().toString("yyyy.MM.dd")<< endl;
           }

       } else {

           qDebug() << "Ошибка открытия файла";
       }
    file.close();
}



void Plot::updateChart(QList<Data> *dataFromDb, DB *dbase1, QValueAxis *axisY, QDateTimeAxis *axisX,
                  QSpinBox *lePickedProd, QDateEdit *leFirstDate, QDateEdit *leSecondDate,QMap<QDateTime,qint32> *mapSaless) {
    ser = new QSplineSeries();

    QList<QDate> endDAte = {leFirstDate->date(), leSecondDate->date()};
    QList<QDateTime> dates;
    QList<qint32> values;
    qint32 pickedprod = lePickedProd->value();
   *mapSaless =  getNumSalesProd(&pickedprod,dataFromDb,&endDAte);


    for (QMap<QDateTime,qint32>::const_iterator i=mapSaless->begin();i!=mapSaless->end();i++) {
        dates.push_back(i.key());
        values.push_back(i.value());
    }

    *slider_range = dates.size();
    sliderBase->setRange(0,*slider_range);
    chart->removeAllSeries();
    chart->series().clear();

    for (int i = 0; i < dates.size(); ++i) {
        ser->append(dates[i].toMSecsSinceEpoch(), values[i]);
    }

    chart->addSeries(ser);

    qint32 a = lePickedProd->value();
    axisY->setTitleText("продажи \"" + dbase1->getProductName(&a) + "\"");
    axisX->setRange(dates.first(), dates.last());
        axisY->setRange(*std::min_element(values.begin(), values.end()), *std::max_element(values.begin(), values.end()));

}


void Plot::lineMove(QMap<QDateTime,qint32> *mapSaless) {

    line->setLine(width()/23.1+sliderBase->value()*((width()-2*width()/23)/(*slider_range*2.187)), height()*0.078,
                              width()/23.2+sliderBase->value()*((width()-2*width()/23)/(*slider_range*2.187)),  height()/1.15);
    labelValue->move(width()/25.5+sliderBase->value()*((width()-2*width()/23)/(*slider_range*2.187)),height()/21);

        labelValue->setText("<p style=\"color: rgb(0, 0, 0)\">"+QString::number((((mapSaless->begin()+sliderBase->value()).value())+1))+"</p>");
    //points.at(sliderBase->value()).y()

}

void Plot::lineMoveNN(QMap<QDateTime,float> *mapSaless) {


    lineNN->setLine(width()/19.6+sliderNN->value()*((width()-2*width()/23)/(*slider_rangeNN*2.2)), height()*0.078,
                              width()/19.6+sliderNN->value()*((width()-2*width()/23)/(*slider_rangeNN*2.2)),  height()/1.15);
    labelValueNN->move(width()/23.5+sliderNN->value()*((width()-2*width()/23)/(*slider_rangeNN*2.2)),height()/21);
    qfloat16 g = std::abs((mapSaless->begin()+sliderNN->value()).value()+1);

        labelValueNN->setText("<p style=\"color: rgb(0, 0, 0)\">"+QString::number(g,'f',2)+"</p>");

}
