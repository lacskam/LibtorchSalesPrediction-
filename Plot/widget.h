#ifndef WIDGET_H
#define WIDGET_H
#include<QHBoxLayout>
#include<QLabel>
#include<QPushButton>
#include<QDateEdit>
#include<QLineEdit>

class Widget : public QWidget
{
    Q_OBJECT
    public:
        Widget(QWidget *parent = 0);
        ~Widget();

    private:
        QHBoxLayout *hb = NULL;

        QDateEdit *leFirstDate = NULL;
        QDateEdit *leSecondDate = NULL;
        QLineEdit *lePickedProd = NULL;

    private slots:
        void update();



};
#endif // WIDGET_H
