#include "addpatientdialog.h"
#include "ui_addpatientdialog.h"
#include "patientdialog.h"
#include<QTextCodec>
#include<QFile>
#include<QDebug>
#include <QMessageBox>
#include <QFileDialog>

AddPatientDialog::AddPatientDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddPatientDialog)
{
    ui->setupUi(this);
    Picture_Path = "";
}

AddPatientDialog::~AddPatientDialog()
{
    delete ui;
}

void AddPatientDialog::on_pushButton_clicked()  //保存
{
    QTextCodec *codec = QTextCodec::codecForLocale();
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    QString text1 = ui->textEdit_11->document()->toPlainText();
    QString text2 = ui->textEdit_10->document()->toPlainText();
    QString text3 = ui->textEdit->document()->toPlainText();
    QString text4 = ui->textEdit_2->document()->toPlainText();
    QString text5 = ui->textEdit_3->document()->toPlainText();
    QString text6 = ui->textEdit_4->document()->toPlainText();
    QString text7 = ui->textEdit_5->document()->toPlainText();
    QString text8 = ui->textEdit_6->document()->toPlainText();
    QString text9 = ui->textEdit_7->document()->toPlainText();
    QString text10 = ui->textEdit_8->document()->toPlainText();
    QString text11 = ui->textEdit_9->document()->toPlainText();
    if (text1 == "" || text2 == "" || text3 == "" || text4 == "" ||
    	text5 == "" || text6 == "" || text7 == "" || text8 == "" || 
        text9 == "" || text10 == "" || text11 == "" || Picture_Path == "")
    {
        qDebug() << "Null" << endl;
        QMessageBox::critical(NULL, QObject::tr("错误"), QObject::tr("病人的所有信息不能为空\n没有请填写无"), QMessageBox::Yes, QMessageBox::Yes);
    }
    else 
    {
        QFile qf("Patient.txt");
	    if(!qf.open(QIODevice::ReadWrite | QIODevice::Text))
	    {
	        qDebug()<<"Can't open the file!"<<endl;
	    }
	    else
	    {
	        QTextStream stream(&qf);
	        stream.seek(qf.size());
	        stream << text1 << " "
	               << text2 << " "
	               << text3 << " "
	               << text4 << " "
	               << text5 << " "
	               << text6 << " "
	               << text7 << " "
	               << text8 << " "
	               << text9 << " "
	               << text10 << " "
                   << text11 << " "
                   << Picture_Path <<"\n";
	    }
	    qf.close();
        this->close();
	    PatientDialog ob;
	    ob.exec();

    }
}

void AddPatientDialog::on_pushButton_3_clicked()
{
    Picture_Path = QFileDialog::getOpenFileName();
    ui->label_13->setPixmap(Picture_Path);
}
