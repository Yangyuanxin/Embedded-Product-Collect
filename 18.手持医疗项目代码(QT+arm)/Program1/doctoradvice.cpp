#include "doctoradvice.h"
#include "ui_doctoradvice.h"
#include <QTextStream>
#include <QFile>
#include <QDebug>
#include <patientdialog.h>
QString name1;
DoctorAdvice::DoctorAdvice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DoctorAdvice)
{
    ui->setupUi(this);
    QString file_name = name1 + "_advice.txt";
    qDebug() << file_name;
    QFile qf(file_name);
    if(!qf.open(QIODevice::ReadWrite | QIODevice::Text))//设置方式
    {
        qDebug()<<"Can't open the file!"<<endl;
    }
    QTextStream stream(&qf);
    ui->plainTextEdit->setPlainText(stream.readAll());
}

DoctorAdvice::~DoctorAdvice()
{
    delete ui;
}

void DoctorAdvice::on_pushButton_clicked()
{
    this->close();
}

void DoctorAdvice::on_pushButton_2_clicked()//save doctor advice
{
    qDebug()  <<"name" << name << endl;
    QString file_name = name + "_advice.txt";
    qDebug() << file_name;
    QFile::remove(file_name);
    QFile qf(file_name);
    if(!qf.open(QIODevice::ReadWrite | QIODevice::Text))//设置方式
    {
        qDebug()<<"Can't open the file!"<<endl;
    }
    QTextStream stream(&qf);
    stream.seek(0);
    stream << ui->plainTextEdit->document()->toPlainText();
}
void DoctorAdvice::SetPatientName(QString name)
{
    this->name = name;
}
