#include "doctordialog.h"
#include "ui_doctordialog.h"

DoctorDialog::DoctorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DoctorDialog)
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
        qDebug() << "begin" << endl;
        datasize = -1;
}

DoctorDialog::~DoctorDialog()
{
    delete ui;
}

void DoctorDialog::on_comboBox_activated(int index)
{
    QString temp_index = QString::number(index+1);
    this->tcpSocket->write(temp_index.toLocal8Bit());
}
void DoctorDialog::TCPConnect()
{
    tcpSocket->abort();
    tcpSocket->connectToHost("192.168.1.15",6666);
}
void DoctorDialog::revData()
{
    qDebug() << "ababab" << datasize << endl;
    if(datasize == 0)
    {
        QDataStream in(tcpSocket);
        in.setVersion( QDataStream::Qt_4_6);
        if(tcpSocket->bytesAvailable() < sizeof(quint32))
        {
            return;
        }
        in>>datasize;
    }
    if(datasize > tcpSocket->bytesAvailable())
    {
        return;
    }
    displayvideo();
    QByteArray datas = tcpSocket->readAll();//接收字符串数据。
    if(tr(datas.data()) == tr("hello"))
    {
        qDebug() << "hello" << endl;
        this->tcpSocket->write("doctor");
    }
    else
    {
        qDebug() << tr(datas.data()) << endl;
        QStringList list =tr(datas.data()).split(" ",QString::SkipEmptyParts);//进行分割
        QStringList::Iterator it = list.begin();
        if(*it != tr("select"))
        {
            ui->comboBox->addItem(*it);
            it++;
            ui->textEdit->setText(*it);
            it++;
            ui->textEdit_2->setText(*it);
            it++;
            ui->textEdit_3->setText(*it);
            it++;
            ui->textEdit_4->setText(*it);
            it++;
            while(++it != list.end())
            {
                qDebug() << *it << endl;
                ui->comboBox->addItem(*it);
                it += 5;
            }
        }
        else
        {
            it+= 2;
            ui->textEdit->setText(*it);
            it++;
            ui->textEdit_2->setText(*it);
            it++;
            ui->textEdit_3->setText(*it);
            it++;
            ui->textEdit_4->setText(*it);
        }
    }

}
void DoctorDialog::displayError(QAbstractSocket::SocketError)
{
   qDebug()<<tcpSocket->errorString();
   tcpSocket->close();
}
void DoctorDialog::displayvideo()
{
        qDebug()  <<"a"<< datasize << endl;
        QByteArray Data = tcpSocket->read(datasize);
        QBuffer buffer(&Data);
        buffer.open( QIODevice::ReadOnly);

        QImageReader reader(&buffer, "png");
        QImage image = reader.read();

        ui->label_6->setScaledContents(true);
        ui->label_6->setPixmap(QPixmap::fromImage(image,Qt::AutoColor));

       // if(datasize != 0) ui->label_2->setText(QString::number(datasize));

        //将datasize图片大小信息重置0为下一接收做准备。

        datasize = 0;
        qDebug() << datasize << endl;
}
