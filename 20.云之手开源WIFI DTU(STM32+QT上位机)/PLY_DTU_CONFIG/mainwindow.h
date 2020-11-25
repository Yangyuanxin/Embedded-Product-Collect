#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QString>
#include <QThread>
#include <QMainWindow>
#include <QSerialPort>        //提供访问串口的功能
#include <QSerialPortInfo>    //提供系统中存在的串口的信息

#include "gobal_para.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void Init_Serial_Para();
    void Init_Device_Mode_Para();
    void Init_Esp8266_Mode_Para();


    void DTU_MODE_SELECT_CMD_REPLY_PROCESS();
    void DTU_FOR_ESP8266_CMD_REPLY_PROCESS();

private slots:


    void Update_Serial_Port(QString Currnet_Select);
    void Update_Serial_Baurdrate(QString Currnet_Select);
    void Update_Serial_ParityBit(QString Currnet_Select);
    void Update_Serial_DataBit(QString Currnet_Select);
    void Update_Serial_StopBit(QString Currnet_Select);

    void Update_Wifi_Mode_Select(QString Currnet_Select);
    void Update_Wifi_Connect_Type_Select(QString Currnet_Select);

    void on_Connect_Device_clicked();
    void on_Main_Exit_clicked();
    void on_Setting_Mode_clicked();
    void on_Find_Mode_clicked();
    void on_DTU_DEVICE_MODE_BACK_clicked();
    void on_DTU_DEVICE_MODE_EXIT_clicked();


    void dtu_reply_process();
    void on_Setting_Wifi_Mode_clicked();
    void on_Setting_Connect_AP_clicked();
    void on_Setting_SoftAP_clicked();
    void on_Setting_Server_Connect_clicked();
    void on_Find_Setting_Wifi_Mode_clicked();
    void on_Find_Current_Ap_clicked();
    void on_Find_Current_SortAp_clicked();
    void on_Return_Dtu_Mode_Slect_Page_clicked();

    void on_Select_Mode_currentIndexChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    QString buf ;
    int Mode ;
    int Wifi_Mode ;
    QSerialPort *Serial ;
    QString Serial_Port , Serial_Bauardate,Serial_PariteyBit,Serial_DataBit,Serial_StopBit ;

    /*当前模式、产品型号*/
    QString Current_Mode , Product_Model;
    /*ESP8266模式*/
    QString Esp8266_Wifi_Mode , Esp8266_Wifi_Connect_Type;
};

#endif // MAINWINDOW_H

