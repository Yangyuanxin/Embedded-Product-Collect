#include "doctordialog.h"
#include "ui_doctordialog.h"
#include <QDebug>
#include <QFile>
#include <QTextCodec>
#include <QString>
#include "doctor.h"
#include "adddoctordialog.h"
#include <QMessageBox>
int count = 0;
DoctorDialog::DoctorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DoctorDialog)
{
    ui->setupUi(this);
    QFile qf("Doctor.txt");			//打开文件
    if(!qf.open(QIODevice::ReadWrite | QIODevice::Text))//设置方式
    {
        qDebug()<<"Can't open the file!"<<endl;
    }
    else
    {
        QTextStream stream(&qf);//选择
        ui->comboBox->clear();//清除combobox的内容
        while(!stream.atEnd())
        {
            QString text = stream.readLine();//读取一行数据
            qDebug() << text << endl;
            QStringList list = text.split(" ",QString::SkipEmptyParts);//进行分割
            QStringList::Iterator it = list.begin();
            qDebug() << *it << endl;
            ui->comboBox->addItem(*it);//添加combobox的内容
        }
    }
    if(qf.size())//如果文件中有数据,则把第一个数据写到界面上面
    {
    	QTextStream stream(&qf);
	    stream.seek(0);
	    QString text = stream.readLine();
	    QStringList list = text.split(" ",QString::SkipEmptyParts);
	    QStringList::Iterator it = list.begin();
	    it++;
	    ui->textEdit->setText(*it);
	    it++;
	    ui->textEdit_2->setText(*it);
	    it++;
	    ui->textEdit_3->setText(*it);
	    it++;
	    ui->textEdit_4->setText(*it);
        it++;
        ui->label_7->setPixmap(*it);
    }
    qf.close();

}

DoctorDialog::~DoctorDialog()
{
    delete ui;
}

void DoctorDialog::on_pushButton_2_clicked()
{
    this->close();
}

void DoctorDialog::on_pushButton_clicked()//save_button
{
    QTextCodec *codec = QTextCodec::codecForLocale();
    QTextCodec::setCodecForTr(QTextCodec::codecForLocale());
    QString text;
    QString text1 = ui->comboBox->currentText();
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
            int rowCount = 0;
            do
            {
                rowCount++;
                stream.readLine();
            }while(!stream.atEnd());				//计算文件有几行
            stream.seek(0);
            Doctor *ob[rowCount];
            int number = 0;
            ob[number] = new Doctor();
            while(!stream.atEnd())				//将文件的信息读到对象指针数组中
            {
                text = stream.readLine();
                QStringList list = text.split(" ",QString::SkipEmptyParts);
                QStringList::Iterator it = list.begin();
                ob[number]->SetName(*it);
                it++;
                ob[number]->SetJob(*it);
                it++;
                ob[number]->SetDepartment(*it);
                it++;
                ob[number]->SetTime(*it);
                it++;
                ob[number]->SetExperience(*it);
                it++;
                ob[number]->SetPhoto(*it);
                number++;
                ob[number] = new Doctor();
            }
            /*修改选择的对象*/
            ob[count]->SetName(text1);
            ob[count]->SetJob(text2);
            ob[count]->SetDepartment(text3);
            ob[count]->SetTime(text4);
            ob[count]->SetExperience(text5);
            stream.seek(0);
            number = 0;
            while(number < rowCount)		//保存
            {
                qDebug() << ob[number]->GetName() << endl;
                stream << ob[number]->GetName() << " "
                       << ob[number]->GetJob()<< " "
                       << ob[number]->GetDepartment()<< " "
                       << ob[number]->GetTime()<< " "
                       << ob[number]->GetExperience() << " "
                       << ob[number]->GetPhoto()
                       << "\r\n";
                number++;
            }
        }
        qf.close();
    }
}

void DoctorDialog::on_comboBox_activated(int index)			//点击combobox事件
{
    ui->textEdit->clear();
    ui->textEdit_2->clear();
    ui->textEdit_3->clear();
    ui->textEdit_4->clear();

    QFile qf("Doctor.txt");
    QString text;
    if(!qf.open(QIODevice::ReadWrite | QIODevice::Text))
    {
        qDebug()<<"Can't open the file!"<<endl;
    }
    else
    {
        qDebug() << index << endl;
        QTextStream stream(&qf);
        count = 0;
        int rowCount = 0;
        while(!stream.atEnd())
        {
            rowCount++;
            stream.readLine();
        }
        if (index > rowCount - 1)                   //
    	{
            qf.close();
            count = index;
    		return ;
    	}
        stream.seek(0);
        do
        {
            text = stream.readLine();
            count++;
        }while(count != index + 1);
        count --;                                  //
        QStringList list = text.split(" ");
        QStringList::Iterator it = list.begin();
        it++;
        ui->textEdit->setText(*it);
        it++;
        ui->textEdit_2->setText(*it);
        it++;
        ui->textEdit_3->setText(*it);
        it++;
        ui->textEdit_4->setText(*it);
        it++;
        ui->label_7->setPixmap(*it);
    }
    qf.close();
}

void DoctorDialog::on_pushButton_4_clicked()//add_button
{
    this->close();
    AddDoctorDialog ob;
    ob.exec();
}

void DoctorDialog::on_pushButton_3_clicked()//delete_button
{
    QFile qf("Doctor.txt");
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
        Doctor *ob[rowCount];
        int number = 0;
        ob[number] = new Doctor();
        while(!stream.atEnd())
        {
            QString text = stream.readLine();
            QStringList list = text.split(" ",QString::SkipEmptyParts);
            QStringList::Iterator it = list.begin();
            ob[number]->SetName(*it);
            it++;
            ob[number]->SetJob(*it);
            it++;
            ob[number]->SetDepartment(*it);
            it++;
            ob[number]->SetTime(*it);
            it++;
            ob[number]->SetExperience(*it);
            it++;
            ob[number]->SetPhoto(*it);
            number++;
            ob[number] = new Doctor();
        }
        number = 0;
        qf.remove();
        QFile qf("Doctor.txt");
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
                if(number == count)
                {

                }
                else
                {
                    qDebug() << ob[number]->GetName() << endl;
                    stream << ob[number]->GetName() << " "
                           << ob[number]->GetJob()<< " "
                           << ob[number]->GetDepartment()<< " "
                           << ob[number]->GetTime()<< " "
                           << ob[number]->GetExperience() << " "
                           << ob[number]->GetPhoto()
                           << "\r\n";
                }
                number++;
            }
        }
        qf.close();
        QFile qf1("Doctor.txt");
        QTextStream stream1(&qf1);
        ui->textEdit->clear();
        ui->textEdit_2->clear();
        ui->textEdit_3->clear();
        ui->textEdit_4->clear();
        ui->label_7->clear();
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
            ui->textEdit->setText(*it);
            it++;
            ui->textEdit_2->setText(*it);
            it++;
            ui->textEdit_3->setText(*it);
            it++;
            ui->textEdit_4->setText(*it);
            it++;
            ui->label_7->setPixmap(*it);
        }
        qf1.close();
    }
}
