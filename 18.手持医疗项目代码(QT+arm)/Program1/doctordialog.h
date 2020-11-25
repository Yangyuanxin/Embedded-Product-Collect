#ifndef DOCTORDIALOG_H
#define DOCTORDIALOG_H

#include <QDialog>

namespace Ui {
class DoctorDialog;
}

class DoctorDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit DoctorDialog(QWidget *parent = 0);
    ~DoctorDialog();
    
private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_comboBox_activated(int index);

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::DoctorDialog *ui;
};

#endif // DOCTORDIALOG_H
