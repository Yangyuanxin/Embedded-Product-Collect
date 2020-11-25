#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget {

    Q_OBJECT

public:

    Widget(QWidget *parent = 0 );
    ~Widget();
    void SetPatientName(QString name);
protected:

    virtual void changeEvent(QEvent *e);
    virtual bool eventFilter(QObject *watched, QEvent *e);
    void paintOnWidget(QWidget *w);
    void paintWidget(QWidget *w, int x,int y, int m, int n, int i);

private:

    Ui::Widget *ui;
    int x;
    QString name;
};


#endif
