#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("鹏力云DTU配置软件%1").arg(SOFTWARE_VERSION));
    ui->stackedWidget->setCurrentIndex(MAIN_PAGE);
    /*初始化串口参数*/
    this->Init_Serial_Para();
    connect(ui->Serial_Port,SIGNAL(currentIndexChanged(QString)),this,SLOT(Update_Serial_Port(QString)));
    connect(ui->Serial_Baurdrate,SIGNAL(currentIndexChanged(QString)),this,SLOT(Update_Serial_Baurdrate(QString)));
    connect(ui->Serial_Parity,SIGNAL(currentIndexChanged(QString)),this,SLOT(Update_Serial_ParityBit(QString)));
    connect(ui->Serial_Data_Bit,SIGNAL(currentIndexChanged(QString)),this,SLOT(Update_Serial_DataBit(QString)));
    connect(ui->Serial_Stop_Bit,SIGNAL(currentIndexChanged(QString)),this,SLOT(Update_Serial_StopBit(QString)));
}

/*初始化串口参数*/
void MainWindow::Init_Serial_Para()
{
    /*当前系统可用端口列表*/
    QList<QSerialPortInfo> list = QSerialPortInfo::availablePorts();
    foreach(const QSerialPortInfo &serialportinfo, list)
        ui->Serial_Port->addItem(serialportinfo.portName());
    /*默认选中第0项*/
    Serial_Port = ui->Serial_Port->itemText(0);

    /*波特率列表*/
    QStringList Serial_Baurdrate_List;
    Serial_Baurdrate_List<<"1200"<<"2400"<<"4800"<<"9600"<<"19200"<<"38400"
                        <<"57600"<<"115200";
    ui->Serial_Baurdrate->addItems(Serial_Baurdrate_List);
    /*默认选中115200*/
    Serial_Bauardate = ui->Serial_Baurdrate->itemText(7);
    ui->Serial_Baurdrate->setCurrentText(Serial_Bauardate);

    /*校验位*/
    QStringList Serial_Parity_List ;
    Serial_Parity_List<<"None"<<"Odd"<<"Even"<<"Mark"<<"Space";
    ui->Serial_Parity->addItems(Serial_Parity_List);
    Serial_PariteyBit = ui->Serial_Parity->itemText(0);
    ui->Serial_Parity->setCurrentText(Serial_PariteyBit);

    /*数据位*/
    QStringList Serial_Data_Bit_List ;
    Serial_Data_Bit_List<<"5"<<"6"<<"7"<<"8";
    ui->Serial_Data_Bit->addItems(Serial_Data_Bit_List);
    /*默认选中第0项*/
    Serial_DataBit = ui->Serial_Data_Bit->itemText(3);
    ui->Serial_Data_Bit->setCurrentText(Serial_DataBit);

    /*停止位*/
    QStringList Serial_Stop_Bit_List ;
    Serial_Stop_Bit_List<<"1"<<"2"<<"3";
    ui->Serial_Stop_Bit->addItems(Serial_Stop_Bit_List);
    /*默认选中第0项*/
    Serial_StopBit = ui->Serial_Stop_Bit->itemText(0);
    ui->Serial_Stop_Bit->setCurrentText(Serial_StopBit);
}

/*更新选中的端口项*/
void MainWindow::Update_Serial_Port(QString Currnet_Select)
{
    Serial_Port = Currnet_Select ;
    qDebug() << "当前选中端口号:" << Serial_Port ;
}

/*更新选中的波特率项*/
void MainWindow::Update_Serial_Baurdrate(QString Currnet_Select)
{
    Serial_Bauardate = Currnet_Select ;
    qDebug() << "当前选中波特率:" << Serial_Bauardate ;
}
/*更新选中的校验位项*/
void MainWindow::Update_Serial_ParityBit(QString Currnet_Select)
{
    Serial_PariteyBit = Currnet_Select ;
    qDebug() << "当前选中校验位:" << Serial_PariteyBit ;
}
/*更新选中数据位项*/
void MainWindow::Update_Serial_DataBit(QString Currnet_Select)
{
    Serial_DataBit = Currnet_Select ;
    qDebug() << "当前选中数据为:" << Serial_DataBit ;
}
/*更新选中停止位项*/
void MainWindow::Update_Serial_StopBit(QString Currnet_Select)
{
    Serial_StopBit = Currnet_Select ;
    qDebug() << "当前选中停止位:" << Serial_StopBit ;
}

/*连接设备*/
void MainWindow::on_Connect_Device_clicked()
{
    qDebug() << "选中端口号:" << Serial_Port;
    qDebug() << "选中波特率:" << Serial_Bauardate;
    qDebug() << "选中校验位:" << Serial_PariteyBit;
    qDebug() << "选中数据位:" << Serial_DataBit;
    qDebug() << "选中停止位:" << Serial_StopBit;
    /*构造一个对象*/
    Serial = new QSerialPort(Serial_Port);
    /*构造波特率*/
    if(Serial_Bauardate == "1200")
        Serial->setBaudRate(QSerialPort::Baud1200);
    else if(Serial_Bauardate == "2400")
        Serial->setBaudRate(QSerialPort::Baud2400);
    else if(Serial_Bauardate == "4800")
        Serial->setBaudRate(QSerialPort::Baud4800);
    else if(Serial_Bauardate == "9600")
        Serial->setBaudRate(QSerialPort::Baud9600);
    else if(Serial_Bauardate == "19200")
        Serial->setBaudRate(QSerialPort::Baud19200);
    else if(Serial_Bauardate == "38400")
        Serial->setBaudRate(QSerialPort::Baud38400);
    else if(Serial_Bauardate == "57600")
        Serial->setBaudRate(QSerialPort::Baud57600);
    else if(Serial_Bauardate == "115200")
        Serial->setBaudRate(QSerialPort::Baud115200);

    /*构造校验位*/
    if(Serial_PariteyBit == "None")
        Serial->setParity(QSerialPort::NoParity);
    else if(Serial_PariteyBit == "Odd")
        Serial->setParity(QSerialPort::OddParity);
    else if(Serial_PariteyBit == "Even")
        Serial->setParity(QSerialPort::EvenParity);
    else if(Serial_PariteyBit == "Mark")
        Serial->setParity(QSerialPort::MarkParity);
    else if(Serial_PariteyBit == "Space")
        Serial->setParity(QSerialPort::SpaceParity);

    /*构造数据位*/
    if(Serial_DataBit == "5")
        Serial->setDataBits(QSerialPort::Data5);
    if(Serial_DataBit == "6")
        Serial->setDataBits(QSerialPort::Data6); ;
    if(Serial_DataBit == "7")
        Serial->setDataBits(QSerialPort::Data7); ;
    if(Serial_DataBit == "8")
        Serial->setDataBits(QSerialPort::Data8); ;

    /*构造停止位*/
    if(Serial_StopBit == "1")
        Serial->setStopBits(QSerialPort::OneStop);
    if(Serial_DataBit == "2")
        Serial->setStopBits(QSerialPort::TwoStop);
    if(Serial_DataBit == "3")
        Serial->setStopBits(QSerialPort::OneAndHalfStop);

    /*构造流控*/
    Serial->setFlowControl(QSerialPort::NoFlowControl);

    if (!Serial->open(QIODevice::ReadWrite))
        qDebug() << "serial open failed";
    else
    {
        qDebug() << "串口打开成功";
        this->Init_Device_Mode_Para();
        ui->stackedWidget->setCurrentIndex(DEVICE_MODE_PAGE);
    }
}

/*退出软件*/
void MainWindow::on_Main_Exit_clicked()
{
    qApp->exit();
}

MainWindow::~MainWindow()
{
    delete ui;
}














