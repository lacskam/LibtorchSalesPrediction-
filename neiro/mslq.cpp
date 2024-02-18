#include"mslq.h"
#include <vector>

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

bool comp(const std::vector<int>& a, const std::vector<int>& b)
{
    if (a[2] < b[2]) {
         return 1;
    }

    if (a[2] > b[2]) {
         return 0;
    }
    if (a[2] == b[2]) {
        if (a[1] < b[1]) {
            return 1;
         } else if (a[1] > b[1])   {
            return 0;
        } else if(a[1] == b[1]) {
            return 0;
        }
    }

}

void importFullInfo(QList<Data> *data) {
    QFile file("allprod.sex");
    qint32 *a=new qint32;
    QList<QDate> endDAte {QDate(2019,5,1),QDate(2021,8,26)};

    QString tempstr;
    std::vector<std::vector<int>> a1;
    QMap<QDateTime,qint32> temp;
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {


           QTextStream stream(&file);

           for (int prod=1;prod<=153;prod++) {
                *a=prod;

               temp = getNumSalesProd(a,data,&endDAte);


               for (auto i=temp.begin();i!=temp.end();i++) {

                   a1.push_back({prod,i.key().date().day(),i.key().date().month(),i.value()});
                  // stream << QString::number(prod)+" "+ QString::number(i.key().date().day())+" "+ QString::number(i.key().date().month())+" "+ QString::number(i.value())<< endl;
               }

           }
            sort(a1.begin(),a1.end(),comp);
           for (int i=0;i<a1.size();i++) {
                    stream << QString::number(a1[i][0])+" "+ QString::number(a1[i][1])+" "+ QString::number(a1[i][2])+" "+ QString::number(a1[i][3])<< endl;


           }




       } else {

           qDebug() << "Ошибка открытия файла";
       }
    file.close();
}
