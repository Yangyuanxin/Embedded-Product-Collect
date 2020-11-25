#ifndef ADDDOCTORDIALOG_H
#define ADDDOCTORDIALOG_H

#include <QDialog>

namespace Ui {
class AddDoctorDialog;
}

class AddDoctorDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddDoctorDialog(QWidget *parent = 0);
    ~AddDoctorDialog();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::AddDoctorDialog *ui;
    QString Picture_Path;
};

#endif // ADDDOCTORDIALOG_H
