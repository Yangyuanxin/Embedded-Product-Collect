#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "doctordialog.h"
#include "floormapdialog.h"
#include"patientdialog.h"

QString flag = "";
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //显示当前系统时间
    ui->dateTimeEdit->setDisplayFormat("yyyy-MM-dd  hh:mm");
    QDateTime qdt;
    ui->dateTimeEdit->setDateTime(qdt.currentDateTime());

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    this->tcpServer = new QTcpServer(this);
    this->tcpSocket = new QTcpSocket(this);
    Listen();
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(acceptConnection()));
    //newConnection()用于当有客户端访问时发出信号，acceptConnection()信号处理函数
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),
            this,SLOT(displayError(QAbstractSocket::SocketError)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_toolButton_clicked()
{
    DoctorDialog dd;
    dd.exec();
}

void MainWindow::on_toolButton_3_clicked()
{
    FloorMapDialog ob;
    ob.exec();
}

void MainWindow::on_toolButton_2_clicked()
{
     PatientDialog *ob = new PatientDialog();
     ob->show();
}
void MainWindow::Listen()
{
    if(!tcpServer->listen(QHostAddress::Any,6666))
    {
        qDebug()<<tcpServer->errorString();
        close();
        return;
    }
}

void MainWindow::acceptConnection()
{
    tcpSocket = tcpServer->nextPendingConnection();
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(revData()));
    this->tcpSocket->write("hello");
}
void MainWindow::displayError(QAbstractSocket::SocketError)
{
   qDebug()<<tcpSocket->errorString();
   tcpSocket->close();
}
void MainWindow::revData()
{
    qDebug() << "hello"<< endl;
    QByteArray datas = tcpSocket->readAll();//接收字符串数据。
    QString temp;
    QString text;
    if(tr(datas.data()).toInt() && flag == tr("patient"))
    {
        qDebug() << "p1" << endl;
        int count = 1,data = tr(datas.data()).toInt();
        QFile qf("Patient.txt");			//打开文件
        if(!qf.open(QIODevice::ReadWrite | QIODevice::Text))//设置方式
        {
            qDebug()<<"Can't open the file!"<<endl;
        }
        else
        {
            QTextStream stream(&qf);
            while(count < data)
            {
                count++;
                text = stream.readLine();
            }
            text = stream.readLine();
            if(data != 1)
            {
                text.remove(0,1);
            }
            QStringList list = text.split(" ",QString::SkipEmptyParts);
            QStringList::Iterator it = list.begin();

            it += 11;

            QByteArray Data;
            QBuffer buffer;
            QDataStream out(&Data,QIODevice::WriteOnly);
            displayvideo(*it);
            image->save(&buffer,"png");
            out.setVersion(QDataStream::Qt_4_6);
                out<<(quint32)buffer.data().size();
                Data.append(buffer.data());

                tcpSocket->write(Data);

                delete image;
                Data.resize(0);
                buffer.reset();
            text = "select " + text;
            qDebug() << text << endl;
            this->tcpSocket->write(text.toLocal8Bit());
        }
    }
    else if(tr(datas.data()) == tr("patient_name"))
    {
        qDebug() << "p2" << endl;
        QFile qf("Patient.txt");			//打开文件
        if(!qf.open(QIODevice::ReadWrite | QIODevice::Text))//设置方式
        {
            qDebug()<<"Can't open the file!"<<endl;
        }
        else
        {
            QTextStream stream(&qf);
            QString temp = stream.readLine();//读取一行数据
            text += temp;
            qDebug() << text.toLocal8Bit() << endl;
            while(!stream.atEnd())
            {
                QString temp = stream.readLine();//读取一行数据
                temp.replace(0,1," ");
                text += temp;
            }
            qDebug() << text.toLocal8Bit() << endl;
        }
        this->tcpSocket->write(text.toLocal8Bit());
    }
    else if(tr(datas.data()) == tr("patient"))
    {
        qDebug() << "p3" << endl;
        qDebug() << "patient" << endl;
        flag = tr("patient");
        QFile qf("Patient.txt");			//打开文件
        if(!qf.open(QIODevice::ReadWrite | QIODevice::Text))//设置方式
        {
            qDebug()<<"Can't open the file!"<<endl;
        }
        else
        {
            QTextStream stream(&qf);
            QString temp = stream.readLine();//读取一行数据
            text += temp;
            qDebug() << text.toLocal8Bit() << endl;
            while(!stream.atEnd())
            {
                QString temp = stream.readLine();//读取一行数据
                temp.replace(0,1," ");
                text += temp;
            }
            qDebug() << text.toLocal8Bit() << endl;
        }

            QStringList list = text.split(" ",QString::SkipEmptyParts);
            QStringList::Iterator it = list.begin();
            it += 11;
            QByteArray Data;
            QBuffer buffer;
            QDataStream out(&Data,QIODevice::WriteOnly);
            displayvideo(*it);
            image->save(&buffer,"png");
            out.setVersion(QDataStream::Qt_4_6);
                out<<(quint32)buffer.data().size();
                Data.append(buffer.data());
                tcpSocket->write(Data);
                delete image;
                Data.resize(0);
                buffer.reset();
                this->tcpSocket->write(text.toLocal8Bit());
                qDebug() << "test" << text.toLocal8Bit() << endl;
    }
    else if(tr(datas.data()).toInt() && flag == tr("doctor"))
    {
        qDebug() << "d1" << endl;
        int count = 1,data = tr(datas.data()).toInt();
        QFile qf("Doctor.txt");			//打开文件
        if(!qf.open(QIODevice::ReadWrite | QIODevice::Text))//设置方式
        {
            qDebug()<<"Can't open the file!"<<endl;
        }
        else
        {
            QTextStream stream(&qf);
            while(count < data)
            {
                count++;
                text = stream.readLine();
            }
            text = stream.readLine();
            if(data != 1)
            {
                text.remove(0,1);
            }

            QStringList list = text.split(" ",QString::SkipEmptyParts);
            QStringList::Iterator it = list.begin();

            it += 5;

            QByteArray Data;
            QBuffer buffer;
            QDataStream out(&Data,QIODevice::WriteOnly);
            displayvideo(*it);
            image->save(&buffer,"png");
            out.setVersion(QDataStream::Qt_4_6);
                out<<(quint32)buffer.data().size();
                Data.append(buffer.data());

                tcpSocket->write(Data);

                delete image;
                Data.resize(0);
                buffer.reset();
            text = "select " + text;
            this->tcpSocket->write(text.toLocal8Bit());
        }
    }
    else if(tr(datas.data()) == tr("doctor"))
    {
        qDebug() << "d2" << endl;
        flag = tr("doctor");
        QFile qf("Doctor.txt");			//打开文件
        if(!qf.open(QIODevice::ReadWrite | QIODevice::Text))//设置方式
        {
            qDebug()<<"Can't open the file!"<<endl;
        }
        else
        {
            QTextStream stream(&qf);
            QString temp = stream.readLine();//读取一行数据
            text += temp;
            qDebug() << text.toLocal8Bit() << endl;
            while(!stream.atEnd())
            {
                QString temp = stream.readLine();//读取一行数据
                temp.replace(0,1," ");
                text += temp;
            }
            qDebug() << text.toLocal8Bit() << endl;
        }
            QStringList list = text.split(" ",QString::SkipEmptyParts);
            QStringList::Iterator it = list.begin();

            it += 5;

            QByteArray Data;
            QBuffer buffer;
            QDataStream out(&Data,QIODevice::WriteOnly);
            displayvideo(*it);
            image->save(&buffer,"png");
            out.setVersion(QDataStream::Qt_4_6);
                out<<(quint32)buffer.data().size();
                Data.append(buffer.data());
                tcpSocket->write(Data);

                delete image;
                Data.resize(0);
                buffer.reset();

                this->tcpSocket->write(text.toLocal8Bit());
    }
    else
    {
        qDebug() << "else" << endl;
        QString text = tr(datas.data());
        qDebug() << "abcd"<< text << endl;
        QStringList list = text.split(" ",QString::SkipEmptyParts);
        QStringList::Iterator it = list.begin();
        qDebug() << list.size() << endl;
        if(list.size() == 2)
        {
            qDebug() << "*it"<< (*it) << endl;
            QString file_name = (*it) + "_advice.txt";
            QFile qf(file_name);
            if(!qf.open(QIODevice::ReadWrite))
            {
                qDebug()<<"Can't open the file!"<<endl;
                return ;
            }
            QTextStream ts(&qf);
            QString temp = ts.readAll();
            qDebug() << temp << endl;
            this->tcpSocket->write(temp.toLocal8Bit());
        }
        if(list.size() == 7)
        {

            QString file_name = (*it) + "_bs.txt";
            QFile qf(file_name);
            if(!qf.open(QIODevice::ReadWrite))
            {
                qDebug()<<"Can't open the file!"<<endl;
                return ;
            }
            QTextStream ts(&qf);
            ts.seek(qf.size());
            ts << text << "\n";
            qf.close();
        }
        else if(list.size() == 8)
        {
            QString file_name = (*it) + "_bp.txt";
            QFile qf(file_name);
            if(!qf.open(QIODevice::ReadWrite))
            {
                qDebug()<<"Can't open the file!"<<endl;
                return ;
            }
            QTextStream ts(&qf);
            ts.seek(qf.size());
            ts << text << "\n";
            qf.close();
        }
    }
}
void MainWindow::displayvideo(QString it)
{
    FILE *fp;
    char file[10577600] = {0};
    char name[100] = {0};

    //sprintf(name,"tu/%d.png",count++);

    fp = fopen(it.toLocal8Bit().data(),"rb");
    qDebug() << it.toLocal8Bit().data() << endl;
    fread(&file,105776,1,fp);

    fclose(fp);

    //if(count == 11) count = 1;

    image = new QImage((unsigned char*)file,0,0,QImage::Format_RGB16);
    image->loadFromData((unsigned char*)file,105776);
    /*
       ui->label_5->setScaledContents(true);
       ui->label_5->setPixmap(QPixmap::fromImage(*image,Qt::AutoColor));
    */
}

