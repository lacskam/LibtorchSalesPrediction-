#include "plot.h"
#include "mslq.h"
#include <algorithm>
#include <QWidget>



Plot::Plot(QWidget *parent, QList<Data> *dataFromDb, qint32 *pickedprod, QList<QDate> *endDAte, DB *dbase1) : QWidget(parent) {
    QVBoxLayout *vb = new QVBoxLayout(this);
    QHBoxLayout *hb = new QHBoxLayout;

    QDateEdit *leFirstDate = new QDateEdit;
    hb->addWidget(leFirstDate);

    QDateEdit *leSecondDate = new QDateEdit;
    hb->addWidget(leSecondDate);

    QSpinBox *lePickedProd = new QSpinBox;
    lePickedProd->setRange(0, 153);
    hb->addWidget(lePickedProd);

    QValueAxis *axisY = new QValueAxis();
    QDateTimeAxis *axisX = new QDateTimeAxis();

    QMap<QDateTime,qint32> *mapSaless = new QMap<QDateTime,qint32>;

    QPushButton *btn = new QPushButton("Plot");
    connect(btn, &QPushButton::clicked, this, [=]() {
        update(dataFromDb, dbase1, axisY, axisX, lePickedProd, leFirstDate, leSecondDate,mapSaless);
    });
    hb->addWidget(btn);

    QPushButton *btnGetFile = new QPushButton("Get flie");
    connect(btnGetFile, &QPushButton::clicked, this, [=]() {
       getfile(lePickedProd,mapSaless);
    });
    QPushButton *btnOpenFile = new QPushButton("Open flie");
    connect(btnOpenFile, &QPushButton::clicked, this, [=]() {
       openfile(axisY, axisX);
    });
     hb->addWidget(btnGetFile);
     hb->addWidget(btnOpenFile);

    chart = new QChart();

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

    QSplineSeries *series = new QSplineSeries();

    for (int i = 0; i < dates.size(); ++i) {
        series->append(dates[i].toMSecsSinceEpoch(), values[i]);
    }



    axisX->setTickCount(22);
    axisX->setFormat("dd.MM.yyyy");

    chart->addSeries(series);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    axisX->setTitleText("дата");
    axisY->setTitleText("продажи \"" + dbase1->getProductName(pickedprod) + "\"");

    chartView = new QChartView(this->chart);
    vb->addWidget(chartView);
    vb->addLayout(hb);
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
        QSplineSeries *ser = new QSplineSeries();
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

void Plot::update(QList<Data> *dataFromDb, DB *dbase1, QValueAxis *axisY, QDateTimeAxis *axisX,
                  QSpinBox *lePickedProd, QDateEdit *leFirstDate, QDateEdit *leSecondDate,QMap<QDateTime,qint32> *mapSaless) {
    QSplineSeries *ser = new QSplineSeries();

    QList<QDate> endDAte = {leFirstDate->date(), leSecondDate->date()};
    QList<QDateTime> dates;
    QList<qint32> values;
    qint32 pickedprod = lePickedProd->value();
   *mapSaless =  getNumSalesProd(&pickedprod,dataFromDb,&endDAte);


    for (QMap<QDateTime,qint32>::const_iterator i=mapSaless->begin();i!=mapSaless->end();i++) {
        dates.push_back(i.key());
        values.push_back(i.value());
    }
\

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


