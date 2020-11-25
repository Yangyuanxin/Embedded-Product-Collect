#ifndef WIDGET123_H
#define WIDGET123_H

#include <QWidget>

namespace Ui {
class widget123;
}

class widget123 : public QWidget
{
    Q_OBJECT

public:
    explicit widget123(QWidget *parent = 0);
    ~widget123();

private:
    Ui::widget123 *ui;
};

#endif // WIDGET123_H
