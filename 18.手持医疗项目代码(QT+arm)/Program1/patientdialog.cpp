#include "patientdialog.h"
#include "ui_patientdialog.h"
#include"addpatientdialog.h"
#include <QTextCodec>
#include <QFile>
#include <QDebug>
#include <patient.h>
#include <QMessageBox>
#include "widget.h"
#include <QApplication>
#include "widget2.h"
#include "doctoradvice.h"
int count1 = 0;
extern QString name1;
PatientDialog::PatientDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PatientDialog)
{
    ui->setupUi(this);
    QFile qf("Patient.txt");
    if(!qf.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
    }

    else
    {
        QTextStream stream(&qf);
        ui->comboBox->clear();
        while(!stream.atEnd())
        {
            QString text = stream.readLine();
            QStringList list = text.split(" ");
            QStringList::Iterator it = list.begin();
            ui->comboBox->addItem(*it);
        }
    }
    if(qf.size())
    {
        QTextStream stream(&qf);
        stream.seek(0);
        QString text = stream.readLine();
        QStringList list = text.split(" ");
        QStringList::Iterator it = list.begin();
        it++;
        ui->textEdit_10->setText(*it);
        it++;
        ui->textEdit->setText(*it);
        it++;
        ui->textEdit_2->setText(*it);
        it++;
        ui->textEdit_3->setText(*it);
        it++;
        ui->textEdit_4->setText(*it);
        it++;
        ui->textEdit_5->setText(*it);
        it++;
        ui->textEdit_6->setText(*it);
        it++;
        ui->textEdit_7->setText(*it);
        it++;
        ui->textEdit_8->setText(*it);
        it++;
        ui->textEdit_9->setText(*it);
        it++;
        ui->label_13->setPixmap(*it);
    }
    qf.close();
}

PatientDialog::~PatientDialog()
{
    delete ui;
}

void PatientDialog::on_pushButton_3_clicked()//add_button
{
    this->close();
    AddPatientDialog apd;
    apd.exec();
}

void PatientDialog::on_comboBox_activated(int index)
{

    ui->textEdit->clear();
    ui->textEdit_2->clear();
    ui->textEdit_3->clear();
    ui->textEdit_4->clear();
    ui->textEdit_5->clear();
    ui->textEdit_6->clear();
    ui->textEdit_7->clear();
    ui->textEdit_8->clear();
    ui->textEdit_9->clear();
    ui->textEdit_10->clear();
    ui->label_13->clear();
    QFile qf("Patient.txt");
    QString text;
    if(!qf.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
    }
    else
    {
        qDebug() << index << endl;
        QTextStream stream(&qf);
        count1 = 0;
        int rowCount = 0;
        while(!stream.atEnd())
        {
            rowCount++;
            stream.readLine();
        }
        if (index > rowCount - 1)                   //
        {
            qf.close();
            count1 = index;
            return ;
        }
        stream.seek(0);
        do
        {
            text = stream.readLine();
            count1++;
        }while(count1 != index + 1);
        count1 --;                                  //
        QStringList list = text.split(" ");
        QStringList::Iterator it = list.begin();
        it++;
        ui->textEdit_10->setText(*it);
        it++;
        ui->textEdit->setText(*it);
        it++;
        ui->textEdit_2->setText(*it);
        it++;
        ui->textEdit_3->setText(*it);
        it++;
        ui->textEdit_4->setText(*it);
        it++;
        ui->textEdit_5->setText(*it);
        it++;
        ui->textEdit_6->setText(*it);
        it++;
        ui->textEdit_7->setText(*it);
        it++;
        ui->textEdit_8->setText(*it);
        it++;
        ui->textEdit_9->setText(*it);
        it++;
        ui->label_13->setPixmap(*it);
    }
    qf.close();
}

void PatientDialog::on_pushButton_clicked()//save_button
{
    QTextCodec *codec = QTextCodec::codecForLocale();
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    QString text;
    QString text1 = ui->comboBox->currentText();
    QString text2 = ui->textEdit->document()->toPlainText();
    QString text3 = ui->textEdit_2->document()->toPlainText();
    QString text4 = ui->textEdit_3->document()->toPlainText();
    QString text5 = ui->textEdit_4->document()->toPlainText();
    QString text6 = ui->textEdit_5->document()->toPlainText();
    QString text7 = ui->textEdit_6->document()->toPlainText();
    QString text8 = ui->textEdit_7->document()->toPlainText();
    QString text9 = ui->textEdit_8->document()->toPlainText();
    QString text10 = ui->textEdit_9->document()->toPlainText();
    QString text11 = ui->textEdit_10->document()->toPlainText();
    if (text1 == "" || text2 == "" || text3 == "" || text4 == "" ||
        text5 == "" || text6 == "" || text7 == "" || text8 == "" ||
        text9 == "" || text10 == "" || text11 == "" )
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
            int rowCount = 0;
            do
            {
                rowCount++;
                stream.readLine();
            }while(!stream.atEnd());				//计算文件有几行
            qDebug() << rowCount << endl;
            stream.seek(0);
            Patient *ob[rowCount];
            int number = 0;
            ob[number] = new Patient();
            while(!stream.atEnd())				//将文件的信息读到对象指针数组中
            {
                text = stream.readLine();
                QStringList list = text.split(" ",QString::SkipEmptyParts);
                QStringList::Iterator it = list.begin();
                ob[number]->SetName(*it);
                it++;
                ob[number]->SetSex(*it);
                it++;
                ob[number]->SetBirth(*it);
                it++;
                ob[number]->SetAddress(*it);
                it++;
                ob[number]->SetPost(*it);
                it++;
                ob[number]->SetPhone(*it);
                it++;
                ob[number]->SetGuomishi(*it);
                it++;
                ob[number]->SetDate1(*it);
                it++;
                ob[number]->SetDate2(*it);
                it++;
                ob[number]->SetDate3(*it);
                it++;
                ob[number]->SetHistory(*it);
                it++;
                ob[number]->SetPhoto(*it);
                number++;
                ob[number] = new Patient();
            }

            /*修改选择的对象*/
            qDebug() << count1 << endl;
            ob[count1]->SetName(text1);
            ob[count1]->SetBirth(text2);
            ob[count1]->SetAddress(text3);
            ob[count1]->SetPost(text4);
            ob[count1]->SetPhone(text5);
            ob[count1]->SetGuomishi(text6);
            ob[count1]->SetDate1(text7);
            ob[count1]->SetDate2(text8);
            ob[count1]->SetDate3(text9);
            ob[count1]->SetHistory(text10);
            ob[count1]->SetSex(text11);
            stream.seek(0);
            number = 0;
            while(number < rowCount)		//保存
            {
                stream << ob[number]->GetName() << " "
                       << ob[number]->GetSex()<< " "
                       << ob[number]->GetBirth()<< " "
                       << ob[number]->GetAddress()<< " "
                       << ob[number]->GetPost() << " "
                       << ob[number]->GetPhone() << " "
                       << ob[number]->GetGuominshi() << " "
                       << ob[number]->GetDate1() << " "
                       << ob[number]->GetDate2() << " "
                       << ob[number]->GetDate3() << " "
                       << ob[number]->GetHistory() << " "
                       << ob[number]->GetPhoto()
                       << "\n";
                number++;
            }
        }
        qf.close();
    }
}

void PatientDialog::on_pushButton_4_clicked()//delete_button
{
    QFile qf("Patient.txt");
    if(!qf.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
    }
    else
    {
        QTextStream stream(&qf);
        int rowCount = 0;
        do
        {
            rowCount++;
            stream.readLine();
        }while(!stream.atEnd());
        qDebug() << rowCount << endl;
        stream.seek(0);
        Patient *ob[rowCount];
        int number = 0;
        ob[number] = new Patient();
        while(!stream.atEnd())
        {
            QString text = stream.readLine();
            QStringList list = text.split(" ",QString::SkipEmptyParts);
            QStringList::Iterator it = list.begin();
            ob[number]->SetName(*it);
            it++;
            ob[number]->SetSex(*it);
            it++;
            ob[number]->SetBirth(*it);
            it++;
            ob[number]->SetAddress(*it);
            it++;
            ob[number]->SetPost(*it);
            it++;
            ob[number]->SetPhone(*it);
            it++;
            ob[number]->SetGuomishi(*it);
            it++;
            ob[number]->SetDate1(*it);
            it++;
            ob[number]->SetDate2(*it);
            it++;
            ob[number]->SetDate3(*it);
            it++;
            ob[number]->SetHistory(*it);
            it++;
            ob[number]->SetPhoto(*it);
            number++;
            ob[number] = new Patient();
        }
        number = 0;
        qf.remove();
        QFile::remove("Patient.txt");
        QFile qf("Patient.txt");
        if(!qf.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            qDebug()<<"Can't open the file!"<<endl;
        }
        else
        {
            QTextStream stream(&qf);
            stream.seek(0);
            while(number < rowCount)
            {
                if(number == count1)
                {

                }
                else
                {
                    qDebug() << ob[number]->GetName() << endl;
                    stream << ob[number]->GetName() << " "
                           << ob[number]->GetSex()<< " "
                           << ob[number]->GetBirth()<< " "
                           << ob[number]->GetAddress()<< " "
                           << ob[number]->GetPost() << " "
                           << ob[number]->GetPhone() << " "
                           << ob[number]->GetGuominshi() << " "
                           << ob[number]->GetDate1() << " "
                           << ob[number]->GetDate2() << " "
                           << ob[number]->GetDate3() << " "
                           << ob[number]->GetHistory() << " "
                           << ob[number]->GetPhoto()
                           << "\n";
                }
                number++;
            }
        }
        qf.close();
        QFile qf1("Patient.txt");
        QTextStream stream1(&qf1);
        ui->textEdit->clear();
        ui->textEdit_2->clear();
        ui->textEdit_3->clear();
        ui->textEdit_4->clear();
        ui->textEdit_5->clear();
        ui->textEdit_6->clear();
        ui->textEdit_7->clear();
        ui->textEdit_8->clear();
        ui->textEdit_9->clear();
        ui->textEdit_10->clear();
        ui->label_13->clear();
        ui->comboBox->clear();
        if(!qf1.open(QIODevice::ReadWrite | QIODevice::Text))
        {
            qDebug()<<"Can't open the file!"<<endl;
        }
        else
        {
            stream1.seek(0);
            while(!stream1.atEnd())
            {
                QString text = stream1.readLine();
                QStringList list = text.split(" ",QString::SkipEmptyParts);
                QStringList::Iterator it = list.begin();
                ui->comboBox->addItem(*it);
            }
        }
        if(qf1.size())
        {
            stream1.seek(0);
            QString text = stream1.readLine();
            QStringList list = text.split(" ",QString::SkipEmptyParts);
            QStringList::Iterator it = list.begin();
            it++;
            ui->textEdit_10->setText(*it);
            it++;
            ui->textEdit->setText(*it);
            it++;
            ui->textEdit_2->setText(*it);
            it++;
            ui->textEdit_3->setText(*it);
            it++;
            ui->textEdit_4->setText(*it);
            it++;
            ui->textEdit_5->setText(*it);
            it++;
            ui->textEdit_6->setText(*it);
            it++;
            ui->textEdit_7->setText(*it);
            it++;
            ui->textEdit_8->setText(*it);
            it++;
            ui->textEdit_9->setText(*it);
            it++;
            ui->label_13->setPixmap(*it);
        }
        qf1.close();
    }
}

void PatientDialog::on_pushButton_6_clicked()  //血糖
{
    Widget2 *ob = new Widget2();
    ob->setVisible(true);
    QString pn = ui->comboBox->currentText();
    int i = ui->comboBox->currentIndex();
    if(pn == "")
    {
        QMessageBox::critical(NULL, QObject::tr("错误"), QObject::tr("未选中病人"), QMessageBox::Yes, QMessageBox::Yes);
        return ;
    }
    if(i != 0)
    {
        pn.remove(0,1);
    }
        ob->SetPatientName(pn);
        qDebug() << pn << endl;
     ob->show();
}

void PatientDialog::on_pushButton_5_clicked()   //血压
{

    Widget *ob = new Widget();
    ob->setVisible(true);
    QString pn = ui->comboBox->currentText();
    int i = ui->comboBox->currentIndex();
    if(pn == "")
    {
        QMessageBox::critical(NULL, QObject::tr("错误"), QObject::tr("未选中病人"), QMessageBox::Yes, QMessageBox::Yes);
        return ;
    }
    if(i != 0)
    {
        pn.remove(0,1);
    }
    ob->SetPatientName(pn);

    ob->show();
}

void PatientDialog::on_pushButton_7_clicked()
{
    name1 = ui->comboBox->currentText();
    qDebug() << "name1 = " << name1 << endl;
    DoctorAdvice ob;
    ob.setGeometry(0,0,320,240);
    QString pn = ui->comboBox->currentText();
    int i = ui->comboBox->currentIndex();
    if(pn == "")
    {
        QMessageBox::critical(NULL, QObject::tr("错误"), QObject::tr("未选中病人"), QMessageBox::Yes, QMessageBox::Yes);
        return ;
    }
    qDebug()<< "name = " << pn << "i = " << i ;
    if(i != 0)
    {
        pn.remove(0,1);
    }
    ob.SetPatientName(pn);
    ob.exec();
}
QString PatientDialog::GetPatientName()
{
    return name;
}
