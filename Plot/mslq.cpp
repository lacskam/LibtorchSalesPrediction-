#include"mslq.h"

Data::Data(QString *dateStr) {

    date = QDate(dateStr->split("-")[0].toInt(),dateStr->split("-")[1].toInt(),dateStr->split("-")[2].toInt());
}


Data::~Data() {


}


DB::DB() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("1756");
    db.setDatabaseName("bd");
    db.open();

}

DB::~DB() {
    db.close();
}





void DB::getIdProduct() {



    QSqlQuery *query = new QSqlQuery(db);
    query->exec("select soldPosition_id_drink from tbl_soldPosition where id_soldPosition<70000;");
    while (query->next()) {

        idProduct.push_back(query->value(0).toInt());

    }
    delete query;



}


QString DB::getProductName(qint32 *id) {
    QSqlQuery *query = new QSqlQuery(db);
    query->exec("select name_drink from tbl_drink where id_drink="+QString::number(*id)+";");
    query->first();
    QString answ = query->value(0).toString();
    delete query;
    return answ;
}

void DB::getDateProductSale() {



    QSqlQuery *query = new QSqlQuery(db);
    query->exec("select date_soldPosition from tbl_soldPosition where id_soldPosition<70000;");
    while (query->next()) {

        dateProductSale.push_back(query->value(0).toString().split("T")[0]);

    }
    delete query;



}


QList<Data> getDateFromDb(DB *dbase) {

    dbase->getIdProduct();
    dbase->getDateProductSale();
    QList<Data> D;

    QString *tempDate = new QString;
    int i1 =0;

    *tempDate = dbase->dateProductSale[0];
    D.push_back(Data(&dbase->dateProductSale[0]));
    for (int i =0;i<dbase->dateProductSale.size();i++) {
        if (*tempDate==dbase->dateProductSale[i]) {
            D[i1].prod.push_back(dbase->idProduct[i]);
        } else {
            *tempDate = dbase->dateProductSale[i];
            D.push_back(Data(&dbase->dateProductSale[i]));
            i1++;
        }
    }
    delete tempDate;
    return D;
}

bool dateCheker(const QDate *date, QList<QDate> *enddate) {
    if (*date <= enddate->at(1) && *date >= enddate->at(0)) {
        return true;
    }
    return false;
}

QMap<QDateTime,qint32> getNumSalesProd(qint32 *prodId,QList<Data> *dataFromDb, QList<QDate> *endDAte) {
    QMap<QDateTime,qint32> numOfSales;

    for (int i = 0; i < dataFromDb->size(); i++) {
        if (dateCheker(&(dataFromDb->at(i).date), endDAte) && !numOfSales.contains(QDateTime(dataFromDb->at(i).date))) {
            numOfSales.insert(QDateTime(dataFromDb->at(i).date),std::count(dataFromDb->at(i).prod.begin(), dataFromDb->at(i).prod.end(), *prodId));
        }
    }
    return numOfSales;
}

