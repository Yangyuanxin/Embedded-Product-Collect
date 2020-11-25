#include "bpdialog.h"
#include "ui_bpdialog.h"
#include <QFile>
#include <QByteArray>
#include <QDebug>
#include <QDateTime>
#include <QObject>
#include <QMessageBox>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
BPDialog::BPDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BPDialog)
{
    ui->setupUi(this);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    this->tcpSocket = new QTcpSocket(this);
        TCPConnect();//这里我是采用程序启动就自访问服务端（也可根据实际情况采用手动连接手动输入服务端ip的方式。）
        connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(revData()));
        //readyRead()表示服务端发送数据过来即发动信号，接着revData()进行处理。
        connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),
                this,SLOT(displayError(QAbstractSocket::SocketError)));

}

BPDialog::~BPDialog()
{
    delete ui;
}

void BPDialog::on_pushButton_clicked()
{
    int data1,data2;
    data1 = qrand()% 50 + 100;
    data2 = qrand()% 30 + 55;
    ui->textEdit->setText(QString::number(data1));
    ui->textEdit_2->setText(QString::number(data2));
    if(data1 > 139 || data2 < 60)
    {
        int fd;
        fd = ::open("/dev/pwm",0);
        if(fd < 0)
        {
            qDebug() << "Open File Error" << endl;
            exit(0);
        }
        else
        {
            ioctl(fd,1,1000);
            usleep(1000*1000);
            ioctl(fd,0,1000);
        }     
        ::close(fd);
    }

}

void BPDialog::on_pushButton_4_clicked()
{
    this->close();
}

void BPDialog::on_pushButton_3_clicked()
{
    if(ui->comboBox->currentText() == QObject::tr(""))
    {
        QMessageBox::critical(NULL,"error","patient name is null",QMessageBox::Yes,QMessageBox::Yes);
    }
    else if(ui->textEdit->document()->toPlainText() == "" || ui->textEdit->document()->toPlainText() == "")
    {
        QMessageBox::critical(NULL,"error","Blood pressure is null",QMessageBox::Yes,QMessageBox::Yes);
    }
    else
    {
        QDateTime qdt = QDateTime::currentDateTime();
        QString text = qdt.toString();
        qDebug() << text << endl;
        QString ob = ui->comboBox->currentText() + "_bp.txt";
        qDebug() << ob << endl;
        QFile qf(ob);
        if(!qf.open(QIODevice::ReadWrite))
        {
            qDebug()<<"Can't open the file!"<<endl;
            return ;
        }
        QTextStream ts(&qf);
        ts.seek(qf.size());
        ts << text << " "
           << ui->textEdit->document()->toPlainText() << " "
           << ui->textEdit_2->document()->toPlainText() << "\n";
        qf.close();
        QString temp = ui->comboBox->currentText() +" "+ text+" " + ui->textEdit->document()->toPlainText() + " " +ui->textEdit_2->document()->toPlainText();
        this->tcpSocket->write(temp.toLocal8Bit());
    }
}
void BPDialog::TCPConnect()
{
    tcpSocket->abort();
    tcpSocket->connectToHost("192.168.1.15",6666);
}
void BPDialog::displayError(QAbstractSocket::SocketError)
{
   qDebug()<<tcpSocket->errorString();
   tcpSocket->close();
}

void BPDialog::revData()
{
    QByteArray datas = tcpSocket->readAll();//接收字符串数据。
    qDebug() <<"testtest"<< datas.data() << endl;
    if(tr(datas.data()) == tr("hello"))
    {
        qDebug() << "patient" << endl;
        this->tcpSocket->write("patient_name");
    }
    else
    {
        qDebug() << "error" << tr(datas.data()) << endl;
        QStringList list =tr(datas.data()).split(" ",QString::SkipEmptyParts);//进行分割
        QStringList::Iterator it = list.begin();
        while(it != list.end())
        {
                qDebug() << *it << endl;
                ui->comboBox->addItem(*it);
                it += 12;
        }
    }
}
