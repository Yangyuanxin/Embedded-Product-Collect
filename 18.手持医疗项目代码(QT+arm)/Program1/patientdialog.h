#ifndef PATIENTDIALOG_H
#define PATIENTDIALOG_H

#include <QDialog>
#include <QString>

namespace Ui {
class PatientDialog;
}

class PatientDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit PatientDialog(QWidget *parent = 0);
    ~PatientDialog();
    QString GetPatientName();
private slots:
    void on_pushButton_3_clicked();

    void on_comboBox_activated(int index);

    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_7_clicked();


private:
    Ui::PatientDialog *ui;
    QString name;
};

#endif // PATIENTDIALOG_H
