#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QByteArray>
#include <QTextCodec>
#include <QFile>
#include <QTextStream>
#include <QBuffer>
#include<QImage>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_toolButton_clicked();

    void on_toolButton_3_clicked();

    void on_toolButton_2_clicked();
    void Listen();
    void displayError(QAbstractSocket::SocketError);
    void acceptConnection(); //接受客户端连接
    void revData(); //接收来自客户端的数据
    void displayvideo(QString it);
private:
    Ui::MainWindow *ui;
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;
    QImage * image;
};

#endif // MAINWINDOW_H
