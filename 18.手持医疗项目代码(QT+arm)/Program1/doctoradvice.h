#ifndef DOCTORADVICE_H
#define DOCTORADVICE_H

#include <QDialog>

namespace Ui {
class DoctorAdvice;
}

class DoctorAdvice : public QDialog
{
    Q_OBJECT
    
public:
    explicit DoctorAdvice(QWidget *parent = 0);
    ~DoctorAdvice();
        void SetPatientName(QString name);
private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::DoctorAdvice *ui;
    QString name;
};

#endif // DOCTORADVICE_H
