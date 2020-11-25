#ifndef WIDGET2_H
#define WIDGET2_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget2 : public QWidget {

    Q_OBJECT

public:

    Widget2(QWidget *parent = 0 );
    ~Widget2();
        void SetPatientName(QString name);
protected:

    virtual void changeEvent(QEvent *e);
    virtual bool eventFilter(QObject *watched, QEvent *e);
    void paintOnWidget(QWidget *w);
    void paintWidget(QWidget *w, int x,int y, int i);

private:

    Ui::Widget *ui;
    QString name;
};


#endif
