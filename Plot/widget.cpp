#include<widget.h>


Widget::Widget(QObject *parent) : QObject(parent) {
    QHBoxLayout *hb = new QHBoxLayout(*parent);

    QDateEdit *leFirstDate = new QDateEdit;
    hb->addWidget(leFirstDate);

    QDateEdit *leSecondDate = new QDateEdit;
    hb->addWidget(leSecondDate);

    QLineEdit *lePickedProd = new QLineEdit;
    hb->addWidget(lePickedProd);





}
