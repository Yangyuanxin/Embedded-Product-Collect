namespace WindowsFormsApplication1
{
    partial class 串口记录仪
    {
        /// <summary>
        /// 必需的设计器变量。
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 清理所有正在使用的资源。
        /// </summary>
        /// <param name="disposing">如果应释放托管资源，为 true；否则为 false。</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows 窗体设计器生成的代码

        /// <summary>
        /// 设计器支持所需的方法 - 不要
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.label1 = new System.Windows.Forms.Label();
            this.comboBox1 = new System.Windows.Forms.ComboBox();
            this.comboBox2 = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.button_opencom = new System.Windows.Forms.Button();
            this.button_selectfile = new System.Windows.Forms.Button();
            this.button_Download = new System.Windows.Forms.Button();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.button_time = new System.Windows.Forms.Button();
            this.BIN = new System.Windows.Forms.GroupBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.rx = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.BIN.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("宋体", 15F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label1.Location = new System.Drawing.Point(15, 55);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(69, 20);
            this.label1.TabIndex = 1;
            this.label1.Text = "端  口";
            // 
            // comboBox1
            // 
            this.comboBox1.FormattingEnabled = true;
            this.comboBox1.Location = new System.Drawing.Point(90, 55);
            this.comboBox1.Name = "comboBox1";
            this.comboBox1.Size = new System.Drawing.Size(97, 20);
            this.comboBox1.TabIndex = 2;
            // 
            // comboBox2
            // 
            this.comboBox2.FormattingEnabled = true;
            this.comboBox2.Location = new System.Drawing.Point(90, 98);
            this.comboBox2.Name = "comboBox2";
            this.comboBox2.Size = new System.Drawing.Size(97, 20);
            this.comboBox2.TabIndex = 4;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("宋体", 15F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label2.Location = new System.Drawing.Point(15, 98);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(69, 20);
            this.label2.TabIndex = 3;
            this.label2.Text = "波特率";
            // 
            // button_opencom
            // 
            this.button_opencom.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.button_opencom.Location = new System.Drawing.Point(12, 151);
            this.button_opencom.Name = "button_opencom";
            this.button_opencom.Size = new System.Drawing.Size(174, 39);
            this.button_opencom.TabIndex = 16;
            this.button_opencom.Text = "打开串口";
            this.button_opencom.UseVisualStyleBackColor = true;
            this.button_opencom.Click += new System.EventHandler(this.button2_Click);
            // 
            // button_selectfile
            // 
            this.button_selectfile.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.button_selectfile.Location = new System.Drawing.Point(12, 196);
            this.button_selectfile.Name = "button_selectfile";
            this.button_selectfile.Size = new System.Drawing.Size(174, 39);
            this.button_selectfile.TabIndex = 17;
            this.button_selectfile.Text = "选择文件";
            this.button_selectfile.UseVisualStyleBackColor = true;
            this.button_selectfile.Click += new System.EventHandler(this.button1_Click);
            // 
            // button_Download
            // 
            this.button_Download.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.button_Download.Location = new System.Drawing.Point(12, 241);
            this.button_Download.Name = "button_Download";
            this.button_Download.Size = new System.Drawing.Size(174, 39);
            this.button_Download.TabIndex = 18;
            this.button_Download.Text = "下载";
            this.button_Download.UseVisualStyleBackColor = true;
            this.button_Download.Click += new System.EventHandler(this.button_Download_Click);
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(135, 20);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(436, 30);
            this.progressBar1.TabIndex = 27;
            // 
            // serialPort1
            // 
            this.serialPort1.BaudRate = 115200;
            this.serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort1_DataReceived);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.button_time);
            this.groupBox1.Controls.Add(this.button_opencom);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Controls.Add(this.button_Download);
            this.groupBox1.Controls.Add(this.comboBox1);
            this.groupBox1.Controls.Add(this.button_selectfile);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.comboBox2);
            this.groupBox1.Location = new System.Drawing.Point(12, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(199, 365);
            this.groupBox1.TabIndex = 28;
            this.groupBox1.TabStop = false;
            // 
            // button_time
            // 
            this.button_time.Font = new System.Drawing.Font("宋体", 9F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.button_time.Location = new System.Drawing.Point(13, 286);
            this.button_time.Name = "button_time";
            this.button_time.Size = new System.Drawing.Size(174, 39);
            this.button_time.TabIndex = 19;
            this.button_time.Text = "同步时间";
            this.button_time.UseVisualStyleBackColor = true;
            this.button_time.Click += new System.EventHandler(this.button_time_Click);
            // 
            // BIN
            // 
            this.BIN.Controls.Add(this.textBox1);
            this.BIN.Location = new System.Drawing.Point(217, 21);
            this.BIN.Name = "BIN";
            this.BIN.Size = new System.Drawing.Size(372, 356);
            this.BIN.TabIndex = 29;
            this.BIN.TabStop = false;
            this.BIN.Text = "BIN";
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(6, 20);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox1.Size = new System.Drawing.Size(360, 330);
            this.textBox1.TabIndex = 0;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.rx);
            this.groupBox3.Controls.Add(this.label4);
            this.groupBox3.Controls.Add(this.label3);
            this.groupBox3.Controls.Add(this.progressBar1);
            this.groupBox3.Location = new System.Drawing.Point(12, 383);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(577, 67);
            this.groupBox3.TabIndex = 30;
            this.groupBox3.TabStop = false;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(58, 38);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(41, 12);
            this.label4.TabIndex = 29;
            this.label4.Text = "label4";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("宋体", 10.5F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(134)));
            this.label3.Location = new System.Drawing.Point(10, 36);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(42, 14);
            this.label3.TabIndex = 28;
            this.label3.Text = "大小:";
            // 
            // timer1
            // 
            this.timer1.Interval = 300;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick);
            // 
            // rx
            // 
            this.rx.AutoSize = true;
            this.rx.Location = new System.Drawing.Point(17, 17);
            this.rx.Name = "rx";
            this.rx.Size = new System.Drawing.Size(41, 12);
            this.rx.TabIndex = 30;
            this.rx.Text = "label5";
            // 
            // 串口记录仪
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.ActiveCaption;
            this.ClientSize = new System.Drawing.Size(601, 462);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.BIN);
            this.Controls.Add(this.groupBox1);
            this.Name = "串口记录仪";
            this.Text = "博硕串口记录仪";
            this.Load += new System.EventHandler(this.IAP串口下载器_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.BIN.ResumeLayout(false);
            this.BIN.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.ComboBox comboBox1;
        private System.Windows.Forms.ComboBox comboBox2;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button button_opencom;
        private System.Windows.Forms.Button button_selectfile;
        private System.Windows.Forms.Button button_Download;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.GroupBox BIN;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Button button_time;
        private System.Windows.Forms.Label rx;
    }
}

