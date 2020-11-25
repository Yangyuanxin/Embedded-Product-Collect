#include "widget.h"
#include "ui_widget.h"

#include <QtGui/QPainter>
#include <QTextStream>
#include <QFile>
#include <QDebug>
Widget::Widget(QWidget *parent) : QWidget(parent), ui(new Ui::Widget) {
    ui->setupUi(this);
    this->installEventFilter(this);
}

Widget::~Widget() {

    delete ui;

}

void Widget::changeEvent(QEvent *e) {

    QWidget::changeEvent(e);

    switch (e->type()) {

        case QEvent::LanguageChange:

        ui->retranslateUi(this);

        break;

        default:

        break;
    }
}

bool Widget::eventFilter(QObject *watched, QEvent *e) {

    QString file_name = name + "_bp.txt";
    qDebug() << file_name;
    QFile qf(file_name);
    int count = 1;
    if(!qf.open(QIODevice::ReadWrite | QIODevice::Text))//设置方式
    {
        qDebug()<<"Can't open the file!"<<endl;
    }

    if (watched == this) {

        if (e->type() == QEvent::Paint) {
            paintOnWidget(this);
            int y = 0;
            int n = 0;
            QTextStream stream(&qf);
            while(!stream.atEnd())
            {
                QString text = stream.readLine();
                QStringList list = text.split(" ",QString::SkipEmptyParts);//进行分割
                QStringList::Iterator it = list.begin();

                it += 6;
                QString data1 = *it;
                int x = data1.toInt();
                qDebug() << "data1" << data1.toInt() << endl;
                it++;
                QString data2 = *it;
                int m =data2.toInt();
                qDebug() << "data2" << data2.toInt() << endl;

                paintWidget(this, x, y, m, n, count);
                count++;
                y = x;
                n = m;
            }
            return true;
        }

    }

    return QWidget::eventFilter(watched, e);

}

void Widget::paintOnWidget(QWidget *w) {

    QPainter painter(w);

    QFontMetrics metrics = painter.fontMetrics();

    int textHeight = metrics.ascent() + metrics.descent();

    int leftWidth = metrics.width(tr("200")) + 5;
    int rightWidth = metrics.width(tr("(日)"));
    int width = w->size().width() - leftWidth - rightWidth;
    int height = w->size().height() - 3 * textHeight;

    // 绘制外框

    painter.drawRect(0, 0, w->size().width() -1, w->size().height() - 1);

    //　移动坐标系

    painter.translate(leftWidth, 1.75 * textHeight + height);

    int totalCount = 200;
    int count = 10;        // 分成10成
    float deltaX = (float)width / count;         // x坐标上每分的宽度
    float deltaY = (float)height / count; // y坐标上每分的宽度

    // 画横坐标

    painter.drawLine(0, 0, width, 0);

    for (int i = 1; i <= 10; ++i) {

        QString month = tr("%1日").arg(i);
        int stringWidth = metrics.width(month);

        // 绘制坐标刻度

        painter.drawLine(deltaX * i , 0, deltaX * i , 4);

        // 绘制坐标处的月

        int monthX = deltaX * (i-0.5) + ((deltaX - stringWidth) / 2);
        painter.drawText(monthX, textHeight, month);

    }

        // 画纵坐标

        painter.drawLine(0, 0, 0, -height);
        painter.drawText(-metrics.width(tr("mm")),
                             -(deltaY * count + textHeight / 2 + metrics.descent()),
                             tr("mmHg"));
        for (int i = 1; i <= count; ++i) {
        QString value = QString("%1").arg(i * totalCount / count);
        int stringWidth = metrics.width(value);

        // 绘制坐标刻度

        painter.drawLine(-4, -i * deltaY, 0, -i * deltaY);

        // 绘制坐标值

        painter.drawText(-stringWidth - 4, -(deltaY * i + textHeight / 2 - metrics.ascent()), value);

    }
    painter.setPen(Qt::red);
    painter.drawLine(deltaX*9, -deltaY*10,deltaX*10, -deltaY*10 );
    painter.setPen(Qt::blue);
    painter.drawLine(deltaX*9, -deltaY*9,deltaX*10, -deltaY*9 );

}
void Widget::paintWidget(QWidget *w, int x, int y, int m, int n, int i)
{
    QPainter paint(w);
    QFontMetrics metrics = paint.fontMetrics();

    int textHeight = metrics.ascent() + metrics.descent();
    int leftWidth = metrics.width(tr("200")) + 5;
    int rightWidth = metrics.width(tr("(日)"));
    int width = w->size().width() - leftWidth - rightWidth;
    int height = w->size().height() - 3 * textHeight;

    int count = 10;
    float deltaX = (float)width / count;
    float deltaY = (float)height / count;

    paint.translate(leftWidth, 1.75 * textHeight + height);

    paint.setPen(Qt::red);
    paint.drawLine(deltaX*(i - 1),-deltaY*(y/20.0),deltaX * i,-deltaY*(x/20.0));
    paint.setPen(Qt::blue);
    paint.drawLine(deltaX*(i - 1),-deltaY*(n/20.0),deltaX * i,-deltaY*(m/20.0));
}

void Widget::SetPatientName(QString name)
{
       this->name = name;
}
