#ifndef ADDPATIENTDIALOG_H
#define ADDPATIENTDIALOG_H

#include <QDialog>

namespace Ui {
class AddPatientDialog;
}

class AddPatientDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit AddPatientDialog(QWidget *parent = 0);
    ~AddPatientDialog();
    
private slots:
    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::AddPatientDialog *ui;
    QString Picture_Path;
};

#endif // ADDPATIENTDIALOG_H
