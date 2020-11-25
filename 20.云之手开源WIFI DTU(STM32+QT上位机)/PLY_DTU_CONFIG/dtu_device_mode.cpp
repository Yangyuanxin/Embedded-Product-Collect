#include "mainwindow.h"
#include "ui_mainwindow.h"

/*初始化设备模式参数*/
void MainWindow::Init_Device_Mode_Para()
{
    /*当前模式列表*/
    QStringList Current_Mode_List;
    Current_Mode_List<<"ESP8266模式"<<"ESP32模式"<<"4G模式"<<"2G模式"<<"3G模式"<<"NB模式";
    ui->Select_Mode->addItems(Current_Mode_List);
    /*默认选中ESP8266模式*/
    Current_Mode = ui->Select_Mode->itemText(0);
    ui->Current_Mode_Display->setText(tr("ESP8266模式"));
    connect(this->Serial,SIGNAL(readyRead()),this,SLOT(dtu_reply_process()));
    connect(ui->Select_Mode,SIGNAL(currentIndexChanged(QString)),this,SLOT(Update_Wifi_Mode_Select(QString)));
}

void MainWindow::on_Select_Mode_currentIndexChanged(const QString &arg1)
{
    if(arg1 == tr("ESP8266模式"))
        Mode = 1 ;
    else if(arg1 == tr("ESP32模式"))
        Mode = 2 ;
    else if(arg1 == tr("4G模式"))
        Mode = 3 ;
    else if(arg1 == tr("2G模式"))
        Mode = 4 ;
    else if(arg1 == tr("3G模式"))
        Mode = 5 ;
    else if(arg1 == tr("NB模式"))
        Mode = 6 ;
}


/*DTU消息回复*/
void MainWindow::dtu_reply_process()
{
    if(this->Serial->canReadLine())
    {
        QByteArray temp = this->Serial->readAll();
        if(!temp.isEmpty())
        {
            buf = tr(temp);
            qDebug() <<"接收到回复:" << buf ;
            //DTU模式设置回复处理
            this->DTU_MODE_SELECT_CMD_REPLY_PROCESS();
            //ESP8266/ESP32模式回复处理=======================================
            this->DTU_FOR_ESP8266_CMD_REPLY_PROCESS();
        }
    }
}

//设置模式回复处理
void MainWindow::DTU_MODE_SELECT_CMD_REPLY_PROCESS()
{
    if(buf.contains("+DTUMode:0"))
        ui->mode_select_tip->setText(tr("进入模式失败!"));
    if(buf.contains("+DTUMode:1"))
    {
        ui->mode_select_tip->setText(tr("进入模式成功!"));
        this->Init_Esp8266_Mode_Para();
        ui->stackedWidget->setCurrentIndex(ESP8266_MODE_PAGE);
    }
    if(buf.contains("+DTUMode:2"))
        ui->mode_select_tip->setText(tr("未有此模式!"));
    if(buf.contains("+DTUMode:3"))
        ui->mode_select_tip->setText(tr("未有此品牌模块!"));

    if(buf.contains("+DTUModeState:1,0"))
        ui->Current_Mode_Display->setText(tr("ESP8266模式"));
    if(buf.contains("+DTUModeState:2,0"))
        ui->Current_Mode_Display->setText(tr("ESP32模式"));
    if(buf.contains("+DTUModeState:3,0"))
        ui->Current_Mode_Display->setText(tr("4G模式"));
    if(buf.contains("+DTUModeState:4,0"))
        ui->Current_Mode_Display->setText(tr("2G模式"));
    if(buf.contains("+DTUModeState:5,0"))
        ui->Current_Mode_Display->setText(tr("3G模式"));
    if(buf.contains("+DTUModeState:6,0"))
        ui->Current_Mode_Display->setText(tr("NB模式"));
}




/*确认设置*/
void MainWindow::on_Setting_Mode_clicked()
{
  //  if(0 == Product_Model.length())
  //      Product_Model = "0";
    QString DTUmode_Cmd = QString("AT+DTUMode=%1,0\r\n").arg(Mode);
    qDebug() << DTUmode_Cmd ;
    this->Serial->write(DTUmode_Cmd.toLatin1());
}

/*确认查询*/
void MainWindow::on_Find_Mode_clicked()
{
    QString DTUmode_Cmd = QString("AT+DTUMode?\r\n");
    this->Serial->write(DTUmode_Cmd.toLatin1());
}


/*返回主页面*/
void MainWindow::on_DTU_DEVICE_MODE_BACK_clicked()
{
    /*断开串口*/
    this->Serial->close();
    /*释放内存*/
    delete this->Serial;
    /*切换到主页面*/
    ui->stackedWidget->setCurrentIndex(MAIN_PAGE);
}

/*退出软件*/
void MainWindow::on_DTU_DEVICE_MODE_EXIT_clicked()
{
    qApp->exit();
}
