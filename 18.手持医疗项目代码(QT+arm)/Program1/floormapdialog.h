#ifndef FLOORMAPDIALOG_H
#define FLOORMAPDIALOG_H

#include <QDialog>

namespace Ui {
class FloorMapDialog;
}

class FloorMapDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit FloorMapDialog(QWidget *parent = 0);
    ~FloorMapDialog();
    
private:
    Ui::FloorMapDialog *ui;
};

#endif // FLOORMAPDIALOG_H
