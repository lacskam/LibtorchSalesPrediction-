#ifndef MSQL_H
#define MSQL_H
#include<QtSql>
#include<iostream>
#include<QSqlDatabase>
#include<QSqlQuery>


class DB {
    private:
        QSqlDatabase db;

     public:
        QList<qint32> idProduct;
        QList<QString> dateProductSale;
        DB();
        ~DB();

        void getIdProduct();
        void getDateProductSale();
        QString getProductName(qint32 *id);


};





class Data {
public:


    QDate date;
    QList<qint32> prod;


    Data(QString *dateStr);
    ~Data();



};


QList<Data> getDateFromDb(DB *dbase);

QMap<QDateTime,qint32> getNumSalesProd(qint32 *prodId,QList<Data> *dataFromDb, QList<QDate> *endDAte);

bool dateCheker(const QDate *date, QList<QDate> *enddate);


#endif // MSQL_H
