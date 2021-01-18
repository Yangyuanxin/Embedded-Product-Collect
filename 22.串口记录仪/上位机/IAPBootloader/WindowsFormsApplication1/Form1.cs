using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.IO;

namespace WindowsFormsApplication1
{
    /* 上位机跟单片机传输协议：
*      1字节（低6位本次大小，高8位结束帧置1，置0正常帧，高7位置一握手信号，置0 数据信号） + DATA + 1字节XOR校验  
*      上位机握手DATA：0xaa,0x55 
*      下位机应答DATA: 有效数据的序号 1字节 + 错误码 0x01 :OK; 0x02 错误：重发
*      有效数据的序号总是自增，上位机发现跟自己发送的次数不匹配，自动结束重发。
*/
    public partial class 串口记录仪 : Form
    {
        bool flag_send = false;                 // 发送状态标志
        public byte[] filebuff = null;          // 保存文件字节码
        public UInt32 counter_file = 0;         // 已发送的字节计数器
        public UInt32 seriaNum = 0;             // 发送的序号
        public  byte ENDFILE = 0x80;            // 结束文件
        public  byte CONNECT = 0x40;            // 握手信号  
        public  byte BYTE2SEND = 62;            // 一次发送的字节
        public const byte IS_OK = 0;            // 下位机传输成功
        public const byte IS_ERRO = 0x01;       // 下位机传输失败
        public byte rxok = 0;
        public int rxconter = 0;
        public byte[] byte_s = new byte[64];

        //从sor数组的start处开始将tar数组中从pianyi开始LEN个元素拷贝过来
        public void copyArray(byte[] sor, UInt32 start, byte[] tar, UInt32 pianyi, int len)
        {
            for (int i = 0; i < len; i++)
            {
                sor[i + start] = tar[pianyi + i];
            }
        }
        // 异或校验
        public byte XOR(byte[] data, int len)
        {
            byte xor = 0;
            for (int i = 0; i < len; i++)
            {
                xor ^= data[i];
            }
            return xor;
        }
        //发生数据函数

        void Send(byte[] file)         // fileLength 文件总长度
        {
            byte[] byte_s = new byte[64];
            byte_s[0] = 0;
            if (counter_file < file.Length)               //判断是否继续发送
            {

                if ((file.Length - counter_file) > BYTE2SEND)
                {
                    byte_s[0] = BYTE2SEND;
                }
                else
                {
                    byte_s[0] = (byte)((file.Length - counter_file) | ENDFILE);
                }

                int datalength = byte_s[0] & 0x3f;
                copyArray(byte_s, 1, filebuff, counter_file, datalength);
                byte_s[datalength + 1] = XOR(byte_s, datalength + 1);
                SeriaSendData(byte_s, 0, datalength + 2); // 输出文件
                counter_file += (UInt32)datalength;
                progressBar1.Value = (int)counter_file;             //更新进度条
                byte_s = null;
            }
            else// 结束发送
            {
                counter_file = 0;
            }
            seriaNum++;     // 发送一次序号自增
        }




        public 串口记录仪()
        {
            InitializeComponent();
        }

        
        private void IAP串口下载器_Load(object sender, EventArgs e)
        {
            //扫描可以用的串口
            SearchAndAddSerialToComboBox(serialPort1, comboBox1);

            // 添加波特率
            comboBox2.Items.Add("115200");
            comboBox2.Items.Add("19200");
            comboBox2.Items.Add("9600");
            comboBox2.Text = "115200";
            serialPort1.Encoding = Encoding.GetEncoding("UTF-8");// 串口接收编码

            Control.CheckForIllegalCrossThreadCalls = false;        //不检查错误线程
        }

        //自动搜索可以用的串口
        private void SearchAndAddSerialToComboBox(SerialPort serialPort, ComboBox comboBox)
        {
            comboBox.Items.Clear();
            serialPort.Close();
            for (int i = 1; i < 30; i++)
            {

                serialPort.PortName = "COM" + i.ToString();
                try
                {
                    serialPort.Open();
                    comboBox.Items.Add(serialPort.PortName);
                }
                catch
                {

                }
                serialPort.Close();
            }
            try
            {
                comboBox.Text = comboBox.Items[0].ToString();   // 在端口号选择框中显示第一个可以用的串口
            }
            catch
            {
                MessageBox.Show("没有可以使用的串口", "错误：");
            }

        }

        private void button2_Click(object sender, EventArgs e)
        {
            try
            {
                if (serialPort1.IsOpen == false)    //串口打开状态
                {
                    serialPort1.PortName = comboBox1.Text;
                    serialPort1.BaudRate = Convert.ToInt32(comboBox2.Text);
                    serialPort1.Open();
                    button_opencom.Text = "关闭串口";
                }
                else
                {
                    button_opencom.Text = "打开串口";
                    serialPort1.Close();
                }
            }
            catch
            {
                MessageBox.Show("端口错误，请检查端口", "错误：");
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {

            OpenFileDialog opnDlg = new OpenFileDialog();
            opnDlg.Filter = "所有文件 |*.bin";
            opnDlg.Title = "打开BIN文件";
            opnDlg.ShowHelp = false;

            if (opnDlg.ShowDialog() == DialogResult.OK)         //打开成功
            {
                try
                {
                    FileStream fs = new FileStream(opnDlg.FileName, FileMode.Open, FileAccess.Read, FileShare.Read); // 创建一个文件流
                    filebuff = new byte[fs.Length];                     // 创建一个字节缓存区
                    fs.Read(filebuff, 0, filebuff.Length);              // 从文件流中读取数据到字节缓存区
                    label4.Text = fs.Length.ToString();                // 显示大小
                    progressBar1.Maximum = (Int32)fs.Length;            // 设置进度条的最大值

                }
                catch (Exception exp)
                {
                    MessageBox.Show(exp.Message);
                }
                ShowInText(textBox1, filebuff);                      //数据区显示16进制数 
                counter_file = 0;                                   //计数值清空
                seriaNum = 0;
                progressBar1.Value = 0;                              //进度条清空   

            }
        }

        //将数组filebuff中的字节，显示到textbox上
        private void ShowInText(TextBox textBox, byte[] buff)
        {
            //数据区显示16进制数
            string str = "";
            const string str1 = "0123456789ABCDEF";
            int counter = 0;    //计数器
            foreach (byte dat in buff)
            {
                str += " " + str1[dat >> 4] + str1[dat & 0X0F];// 将字节转换成16进制的字符串
                counter++;
                if (counter > 10000)
                {
                    counter = 0;
                    textBox.AppendText(str);           //将字符显示到数据区    
                    str = "";                           //清空字符串
                }
            }
            textBox.AppendText(str);
            str = null;

        }



        private void button_Download_Click(object sender, EventArgs e)
        {
            if (flag_send == false)                             //如果不是在发送状态
            {
                if (serialPort1.IsOpen)     // 端口是打开的状态
                {
                    if (filebuff == null) MessageBox.Show("请选择一个BIN文件", "警告：");
                    else
                    {
                        counter_file = 0;
                        progressBar1.Value = 0;

                        timer1.Start();     // 开启定时发送功能
                    }
                }
                else
                {

                    MessageBox.Show("请打开串口", "警告：");
                }
            }
            else MessageBox.Show("正在传输文件", "警告：");
        }


        // 定时器中断300ms 
        private void timer1_Tick(object sender, EventArgs e)
        {
            if(rxok == 1)
            {
                rxconter++;
                if (rxconter > 10)
                {
                    rxconter = 0;
                    rx.Text = "   ";
                }
            }

        }

        // 串口发送信息
        private void SeriaSendData(byte[] array, Int32 pianyi, Int32 len)
        {
            try
            {
                serialPort1.Write(array, pianyi, len);
            }
            catch
            {
                serialPort1.Close();
                button_opencom.Text = "打开串口";
            }
        }

        /* 上位机跟单片机传输协议：
*      1字节（低6位本次大小，高8位结束帧置1，置0正常帧，高7位置一握手信号，置0 数据信号） + DATA + 1字节XOR校验  
*      上位机握手DATA：0xaa,0x55 
*      下位机应答DATA: 有效数据的序号 1字节 + 错误码
*      有效数据的序号总是自增，上位机发现跟自己发送的次数不匹配，自动结束重发。
*/
        void TransportProtocol(byte[] array, byte len, byte endfile, byte connect)
        {
            byte[] newbyte = new byte[len + 2];
            newbyte[0] = (byte)(len | endfile | connect);
            copyArray(newbyte, 1, array, 0, len);
            newbyte[len + 1] = XOR(newbyte, len + 1);
            SeriaSendData(newbyte, 0, len + 2);
        }

        // 收到串口数据
        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            byte[] rxbuff = new byte[serialPort1.BytesToRead];
            serialPort1.Read(rxbuff, 0, rxbuff.Length);//将信息读到数组 
            //分析下位机传来的命令
            if (XOR(rxbuff, rxbuff.Length - 1) == rxbuff[rxbuff.Length - 1])    // 校验
            { 
                if(seriaNum == rxbuff[1])   //校验序号
                {
                    if(rxbuff[2] == IS_OK)
                    {
                        MessageBox.Show("RX OK", "RX");
                        //Send(filebuff);
                        rx.Text = "ok!";
                        rxok = 1;

                    }
                    if(rxbuff[2] == IS_ERRO)    //重传
                    {

                    }
                }
                
            }
 /*           switch (rxbuff[0])
            {
                case ReqMessage:
                    if (flag_sendImage == true)
                    {

                        textBox2.Text = counter_image.ToString();//计算已经发生的数据
                        textBox2.AppendText(" 已经发送\r\n");
                        SendImage(ref counter_image, filebuff.Length, filebuff, ref sendMessage, IMAGEFILE);

                    }
                    else
                        if (flag_sendText == true)
                        {
                            textBox2.Text = counter_text.ToString();//计算已经发生的数据
                            textBox2.AppendText(" 已经发送\r\n");
                            SendImage(ref counter_text, textbuff.Length, textbuff, ref sendMessage, TEXTFILE);
                        }
                    break;
                case StopMessage:
                    flag_sendImage = false;
                    flag_sendText = false;
                    MessageBox.Show("从机没有返回信息", "下位机信息");
                    break;
                default:
                    string str = Encoding.Default.GetString(rxbuff, 0, rxbuff.Length);
                    textBox2.AppendText(str);//显示出信息
                    break;
            }
            */

            rxbuff = null;          //释放内存
        }

        // 同步时间
        private void button_time_Click(object sender, EventArgs e)
        {
            String dateString = System.DateTime.Now.ToString();
             byte[] strbuf = new byte[dateString.Length];
            for(UInt16 i = 0;i<dateString.Length;i++) 
                strbuf[i] = (byte)dateString[i];
            TransportProtocol(strbuf,(byte)strbuf.Length,ENDFILE,0);
            rx.Text = "send time";
        }




    }
}
