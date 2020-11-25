#include "mainwindow.h"
#include "ui_mainwindow.h"


//初始化ESP8266模式
void MainWindow::Init_Esp8266_Mode_Para()
{
    /*默认无WIFI模式*/
    Wifi_Mode = 0 ;
    /*wifi模式选择*/
    QStringList Wifi_Mode_List;
    Wifi_Mode_List<<"无WIFI模式"<<"Station模式"<<"SoftAP模式"<<"SoftAP+Station模式";
    ui->Wifi_Mode_Select->addItems(Wifi_Mode_List);
    /*默认选中无WIFI模式*/
    Esp8266_Wifi_Mode = ui->Wifi_Mode_Select->itemText(0);

    QStringList Wifi_Connect_Type_List;
    Wifi_Connect_Type_List << "TCP"<<"UDP"<<"SSL";
    ui->TCP_Type_Select->addItems(Wifi_Connect_Type_List);
    Esp8266_Wifi_Connect_Type = ui->TCP_Type_Select->itemText(0);

    connect(ui->Wifi_Mode_Select,SIGNAL(currentIndexChanged(QString)),this,SLOT(Update_Wifi_Mode_Select(QString)));
    connect(ui->TCP_Type_Select,SIGNAL(currentIndexChanged(QString)),this,SLOT(Update_Wifi_Connect_Type_Select(QString)));
}

/*返回上一级目录*/
void MainWindow::on_Return_Dtu_Mode_Slect_Page_clicked()
{
    ui->stackedWidget->setCurrentIndex(DEVICE_MODE_PAGE);
}

/*更新选中模式*/
void MainWindow::Update_Wifi_Mode_Select(QString Currnet_Select)
{
    Esp8266_Wifi_Mode = Currnet_Select ;
    if(Esp8266_Wifi_Mode == tr("无WIFI模式"))
        Wifi_Mode = 0 ;
    else if(Esp8266_Wifi_Mode == tr("Station模式"))
        Wifi_Mode = 1 ;
    else if(Esp8266_Wifi_Mode == tr("SoftAP模式"))
        Wifi_Mode = 2 ;
    else if(Esp8266_Wifi_Mode == tr("SoftAP+Station模式"))
        Wifi_Mode = 3 ;
}

/*更新选中类型*/
void MainWindow::Update_Wifi_Connect_Type_Select(QString Currnet_Select)
{
    Esp8266_Wifi_Connect_Type = Currnet_Select ;
}


/*设置WIFI模式*/
void MainWindow::on_Setting_Wifi_Mode_clicked()
{
    QString DTUmode_Cmd = QString("AT+DTUWFMode=%1\r\n").arg(Wifi_Mode);
    qDebug() << "WIFI模式:" << DTUmode_Cmd;
    this->Serial->write(DTUmode_Cmd.toLatin1());
}

/*查询设置的WIFI模式*/
void MainWindow::on_Find_Setting_Wifi_Mode_clicked()
{
    QString DTUmode_Cmd = QString("AT+DTUWFMode?\r\n");
    this->Serial->write(DTUmode_Cmd.toLatin1());
}

/*连接AP*/
void MainWindow::on_Setting_Connect_AP_clicked()
{
    QString ssid = ui->JAP_SSID->text();
    QString password = ui->JAP_PWD->text();
    if(ssid.length() == 0 || password.length() == 0)
    {
        ui->Setting_DTU_Tip->setText(tr("SSID或PASSWORD为空!"));
        return ;
    }
    if(ssid.length() > 20 || password.length() > 20)
    {
        ui->Setting_DTU_Tip->setText(tr("SSID或PASSWORD过长!"));
        return ;
    }
    QString DTUmode_Cmd = QString("AT+DTUWFJAP=%1,%2\r\n").arg(ssid).arg(password);
    qDebug() << DTUmode_Cmd;
    this->Serial->write(DTUmode_Cmd.toLatin1());
}

/*查询当前设置的AP*/
void MainWindow::on_Find_Current_Ap_clicked()
{
    QString DTUmode_Cmd = QString("AT+DTUWFJAP?\r\n");
    this->Serial->write(DTUmode_Cmd.toLatin1());
}

/*一键SoftAp配置*/
void MainWindow::on_Setting_SoftAP_clicked()
{
    QString ssid = ui->SAP_SSID->text();
    QString password = ui->SAP_PWD->text();
    if(ssid.length() == 0 || password.length() == 0)
    {
        ui->Setting_DTU_Tip->setText(tr("SSID或PASSWORD为空!"));
        return ;
    }
    if(ssid.length() > 20 || password.length() > 20)
    {
        ui->Setting_DTU_Tip->setText(tr("SSID或PASSWORD过长!"));
        return ;
    }
    QString DTUmode_Cmd = QString("AT+DTUWFSAP=%1,%2\r\n").arg(ssid).arg(password);
    qDebug() << DTUmode_Cmd;
    this->Serial->write(DTUmode_Cmd.toLatin1());
}

/*查询当前SoftAp*/
void MainWindow::on_Find_Current_SortAp_clicked()
{
    QString DTUmode_Cmd = QString("AT+DTUWFSAP?\r\n");
    qDebug() << DTUmode_Cmd;
    this->Serial->write(DTUmode_Cmd.toLatin1());
}

/*建立TCP连接*/
void MainWindow::on_Setting_Server_Connect_clicked()
{
    QString Server_Ip =    ui->Server_Addr->text();
    QString Server_Port =  ui->Server_Port->text();
    if(Server_Ip.length() == 0 || Server_Port.length() == 0)
    {
        ui->Setting_DTU_Tip->setText(tr("IP或端口号为空!"));
        return ;
    }
    if(Server_Ip.length() > 20 || Server_Port.length() > 5)
    {
        ui->Setting_DTU_Tip->setText(tr("IP或端口号过长!"));
        return ;
    }
    QString DTUmode_Cmd = QString("AT+DTUCIPSTART=%1,%2,%3\r\n").arg(Esp8266_Wifi_Connect_Type).arg(Server_Ip).arg(Server_Port);
    this->Serial->write(DTUmode_Cmd.toLatin1());
}

//ESP8266模式回复处理=======================================
void MainWindow::DTU_FOR_ESP8266_CMD_REPLY_PROCESS()
{
    /*********************设置与查询WIFI模式***************************/
    if(buf.contains("+DTUSWFMode:0"))
        ui->Setting_DTU_Tip->setText(tr("进入模式失败"));
    else if(buf.contains("+DTUSWFMode:1"))
        ui->Setting_DTU_Tip->setText(tr("进入模式成功"));

    if(buf.contains("+DTUWFMode?:0"))
        ui->Current_Wifi_Mode->setText(tr("无WIFI模式"));
    else if(buf.contains("+DTUWFMode?:1"))
        ui->Current_Wifi_Mode->setText(tr("Station模式"));
    else if(buf.contains("+DTUWFMode?:2"))
        ui->Current_Wifi_Mode->setText(tr("SoftAP模式"));
    else if(buf.contains("+DTUWFMode?:3"))
        ui->Current_Wifi_Mode->setText(tr("SoftAP+Station模式"));
    /*********************设置与查询WIFI模式***************************/

    if(buf.contains("+DTUWFJAP:0"))
        ui->Setting_DTU_Tip->setText(tr("设置并存储失败"));
    else if(buf.contains("+DTUWFJAP:1"))
        ui->Setting_DTU_Tip->setText(tr("设置并存储成功"));

    if(buf.contains("+DTUWFSAP:0"))
        ui->Setting_DTU_Tip->setText(tr("设置并存储失败"));
    else if(buf.contains("+DTUWFSAP:1"))
        ui->Setting_DTU_Tip->setText(tr("设置并存储成功"));

    if(buf.contains("+DTUSWFMode:0"))
        ui->Setting_DTU_Tip->setText(tr("进入模式失败"));
    else if(buf.contains("+DTUSWFMode:1"))
        ui->Setting_DTU_Tip->setText(tr("进入模式成功"));

    if(buf.contains("+DTUWFSAPSet:0"))
        ui->Setting_DTU_Tip->setText(tr("设置并存储失败"));
    else if(buf.contains("+DTUWFSAPSet:1"))
        ui->Setting_DTU_Tip->setText(tr("设置并存储成功"));


    if(buf.contains("+DTUWFJAP?:"))
    {
        qDebug() <<"接收到回复:" << buf ;
        char buf_ssid[20] = {0};
        char *ptr = strstr(buf.toLatin1(),":");
        char *ptr1 = strstr(buf.toLatin1(),",");
        int len = ptr1-ptr ;
        memcpy(buf_ssid,ptr+1,len-1);
        buf_ssid[len-1] = '\0';
        ui->Current_Ap->setText(tr  ("SSID:%1").arg(buf_ssid));
    }

    if(buf.contains("+DTUWFSAP?:"))
    {
        qDebug() <<"接收到回复:" << buf ;
        char buf_ssid[20] = {0};
        char *ptr = strstr(buf.toLatin1(),":");
        char *ptr1 = strstr(buf.toLatin1(),",");
        int len = ptr1-ptr ;
        memcpy(buf_ssid,ptr+1,len-1);
        buf_ssid[len-1] = '\0';
        ui->Current_SoftAp->setText(tr  ("SSID:%1").arg(buf_ssid));
    }
}


