#include "adddoctordialog.h"
#include "ui_adddoctordialog.h"
#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QString>
#include "doctordialog.h"
#include <QMessageBox>
AddDoctorDialog::AddDoctorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDoctorDialog)
{
    ui->setupUi(this);
}

AddDoctorDialog::~AddDoctorDialog()
{
    delete ui;
}

void AddDoctorDialog::on_pushButton_clicked()
{
    QTextCodec *codec = QTextCodec::codecForLocale();
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    QString text1 = ui->textEdit_5->document()->toPlainText();
    QString text2 = ui->textEdit->document()->toPlainText();
    QString text3 = ui->textEdit_2->document()->toPlainText();
    QString text4 = ui->textEdit_3->document()->toPlainText();
    QString text5 = ui->textEdit_4->document()->toPlainText();
    if (text1 == "" || text2 == "" || text3 == "" || text4 == "" || text5 == "" )
    {
        qDebug() << "Null" << endl;
        QMessageBox::critical(NULL, QObject::tr("错误"), QObject::tr("医生的所有信息不能为空\n没有请填写无"), QMessageBox::Yes, QMessageBox::Yes);
    }
    else
    {
        QFile qf("Doctor.txt");
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
                   << text5 << "\n";
        }
        qf.close();
        DoctorDialog ob;
        ob.exec();
        this->close();
    }
}
