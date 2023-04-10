namespace CarMaster
{
    partial class Form1
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
        /// 设计器支持所需的方法 - 不要修改
        /// 使用代码编辑器修改此方法的内容。
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.motorConnectBtn = new System.Windows.Forms.Button();
            this.runMode = new System.Windows.Forms.CheckBox();
            this.velBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.startBtn = new System.Windows.Forms.Button();
            this.stopBtn = new System.Windows.Forms.Button();
            this.motorCloseBtn = new System.Windows.Forms.Button();
            this.portComboBox = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label3 = new System.Windows.Forms.Label();
            this.baudComboBox = new System.Windows.Forms.ComboBox();
            this.connectLaserBtn = new System.Windows.Forms.Button();
            this.disconnectLaserBtn = new System.Windows.Forms.Button();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.addressComboBox = new System.Windows.Forms.ComboBox();
            this.openLaserBtn = new System.Windows.Forms.Button();
            this.singleDistanceBtn = new System.Windows.Forms.Button();
            this.continuousDistanceBtn = new System.Windows.Forms.Button();
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.button5 = new System.Windows.Forms.Button();
            this.button6 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // motorConnectBtn
            // 
            this.motorConnectBtn.Location = new System.Drawing.Point(12, 12);
            this.motorConnectBtn.Name = "motorConnectBtn";
            this.motorConnectBtn.Size = new System.Drawing.Size(137, 30);
            this.motorConnectBtn.TabIndex = 0;
            this.motorConnectBtn.Text = "打开电机串口";
            this.motorConnectBtn.UseVisualStyleBackColor = true;
            this.motorConnectBtn.Click += new System.EventHandler(this.motorConnectBtn_Click);
            // 
            // runMode
            // 
            this.runMode.AutoSize = true;
            this.runMode.Location = new System.Drawing.Point(12, 58);
            this.runMode.Name = "runMode";
            this.runMode.Size = new System.Drawing.Size(106, 22);
            this.runMode.TabIndex = 1;
            this.runMode.Text = "横向移动";
            this.runMode.UseVisualStyleBackColor = true;
            this.runMode.CheckedChanged += new System.EventHandler(this.runMode_CheckedChanged);
            // 
            // velBox
            // 
            this.velBox.Location = new System.Drawing.Point(172, 91);
            this.velBox.Name = "velBox";
            this.velBox.Size = new System.Drawing.Size(98, 28);
            this.velBox.TabIndex = 2;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(13, 96);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(152, 18);
            this.label1.TabIndex = 3;
            this.label1.Text = "速度设置(step/s)";
            // 
            // startBtn
            // 
            this.startBtn.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.startBtn.Location = new System.Drawing.Point(12, 127);
            this.startBtn.Name = "startBtn";
            this.startBtn.Size = new System.Drawing.Size(80, 29);
            this.startBtn.TabIndex = 4;
            this.startBtn.Text = "运行";
            this.startBtn.UseVisualStyleBackColor = true;
            this.startBtn.Click += new System.EventHandler(this.startBtn_Click);
            // 
            // stopBtn
            // 
            this.stopBtn.Location = new System.Drawing.Point(113, 127);
            this.stopBtn.Name = "stopBtn";
            this.stopBtn.Size = new System.Drawing.Size(80, 29);
            this.stopBtn.TabIndex = 5;
            this.stopBtn.Text = "停止";
            this.stopBtn.UseVisualStyleBackColor = true;
            this.stopBtn.Click += new System.EventHandler(this.stopBtn_Click);
            // 
            // motorCloseBtn
            // 
            this.motorCloseBtn.Location = new System.Drawing.Point(155, 12);
            this.motorCloseBtn.Name = "motorCloseBtn";
            this.motorCloseBtn.Size = new System.Drawing.Size(137, 30);
            this.motorCloseBtn.TabIndex = 6;
            this.motorCloseBtn.Text = "关闭电机串口";
            this.motorCloseBtn.UseVisualStyleBackColor = true;
            this.motorCloseBtn.Click += new System.EventHandler(this.motorCloseBtn_Click);
            // 
            // portComboBox
            // 
            this.portComboBox.FormattingEnabled = true;
            this.portComboBox.Location = new System.Drawing.Point(769, 14);
            this.portComboBox.Name = "portComboBox";
            this.portComboBox.Size = new System.Drawing.Size(121, 26);
            this.portComboBox.TabIndex = 7;
            this.portComboBox.MouseDown += new System.Windows.Forms.MouseEventHandler(this.portComboBox_MouseDown);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(647, 18);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(116, 18);
            this.label2.TabIndex = 8;
            this.label2.Text = "激光测距串口";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(647, 53);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(62, 18);
            this.label3.TabIndex = 9;
            this.label3.Text = "波特率";
            // 
            // baudComboBox
            // 
            this.baudComboBox.FormattingEnabled = true;
            this.baudComboBox.Location = new System.Drawing.Point(769, 49);
            this.baudComboBox.Name = "baudComboBox";
            this.baudComboBox.Size = new System.Drawing.Size(121, 26);
            this.baudComboBox.TabIndex = 10;
            // 
            // connectLaserBtn
            // 
            this.connectLaserBtn.Location = new System.Drawing.Point(938, 13);
            this.connectLaserBtn.Name = "connectLaserBtn";
            this.connectLaserBtn.Size = new System.Drawing.Size(99, 29);
            this.connectLaserBtn.TabIndex = 11;
            this.connectLaserBtn.Text = "连接激光";
            this.connectLaserBtn.UseVisualStyleBackColor = true;
            this.connectLaserBtn.Click += new System.EventHandler(this.connectLaserBtn_Click);
            // 
            // disconnectLaserBtn
            // 
            this.disconnectLaserBtn.Location = new System.Drawing.Point(938, 48);
            this.disconnectLaserBtn.Name = "disconnectLaserBtn";
            this.disconnectLaserBtn.Size = new System.Drawing.Size(99, 29);
            this.disconnectLaserBtn.TabIndex = 12;
            this.disconnectLaserBtn.Text = "断开激光";
            this.disconnectLaserBtn.UseVisualStyleBackColor = true;
            this.disconnectLaserBtn.Click += new System.EventHandler(this.disconnectLaserBtn_Click);
            // 
            // serialPort1
            // 
            this.serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort1_DataReceived);
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(650, 183);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(431, 317);
            this.textBox1.TabIndex = 13;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(647, 90);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(80, 18);
            this.label4.TabIndex = 14;
            this.label4.Text = "激光地址";
            // 
            // addressComboBox
            // 
            this.addressComboBox.FormattingEnabled = true;
            this.addressComboBox.Location = new System.Drawing.Point(769, 86);
            this.addressComboBox.Name = "addressComboBox";
            this.addressComboBox.Size = new System.Drawing.Size(121, 26);
            this.addressComboBox.TabIndex = 15;
            // 
            // openLaserBtn
            // 
            this.openLaserBtn.Location = new System.Drawing.Point(650, 127);
            this.openLaserBtn.Name = "openLaserBtn";
            this.openLaserBtn.Size = new System.Drawing.Size(97, 29);
            this.openLaserBtn.TabIndex = 16;
            this.openLaserBtn.Text = "打开激光";
            this.openLaserBtn.UseVisualStyleBackColor = true;
            this.openLaserBtn.Click += new System.EventHandler(this.openLaserBtn_Click);
            // 
            // singleDistanceBtn
            // 
            this.singleDistanceBtn.Location = new System.Drawing.Point(784, 127);
            this.singleDistanceBtn.Name = "singleDistanceBtn";
            this.singleDistanceBtn.Size = new System.Drawing.Size(97, 29);
            this.singleDistanceBtn.TabIndex = 17;
            this.singleDistanceBtn.Text = "单次测距";
            this.singleDistanceBtn.UseVisualStyleBackColor = true;
            this.singleDistanceBtn.Click += new System.EventHandler(this.singleDistanceBtn_Click);
            // 
            // continuousDistanceBtn
            // 
            this.continuousDistanceBtn.Location = new System.Drawing.Point(918, 127);
            this.continuousDistanceBtn.Name = "continuousDistanceBtn";
            this.continuousDistanceBtn.Size = new System.Drawing.Size(97, 29);
            this.continuousDistanceBtn.TabIndex = 18;
            this.continuousDistanceBtn.Text = "连续测距";
            this.continuousDistanceBtn.UseVisualStyleBackColor = true;
            this.continuousDistanceBtn.Click += new System.EventHandler(this.continuousDistanceBtn_Click);
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(155, 235);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(76, 62);
            this.button1.TabIndex = 19;
            this.button1.Text = "前进";
            this.button1.UseVisualStyleBackColor = true;
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(155, 374);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(76, 62);
            this.button2.TabIndex = 20;
            this.button2.Text = "后退";
            this.button2.UseVisualStyleBackColor = true;
            // 
            // button3
            // 
            this.button3.Location = new System.Drawing.Point(73, 303);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(76, 62);
            this.button3.TabIndex = 21;
            this.button3.Text = "左移";
            this.button3.UseVisualStyleBackColor = true;
            // 
            // button4
            // 
            this.button4.Location = new System.Drawing.Point(241, 303);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(76, 62);
            this.button4.TabIndex = 22;
            this.button4.Text = "右移";
            this.button4.UseVisualStyleBackColor = true;
            // 
            // button5
            // 
            this.button5.Location = new System.Drawing.Point(73, 235);
            this.button5.Name = "button5";
            this.button5.Size = new System.Drawing.Size(76, 62);
            this.button5.TabIndex = 23;
            this.button5.Text = "左转";
            this.button5.UseVisualStyleBackColor = true;
            // 
            // button6
            // 
            this.button6.Location = new System.Drawing.Point(241, 235);
            this.button6.Name = "button6";
            this.button6.Size = new System.Drawing.Size(76, 62);
            this.button6.TabIndex = 24;
            this.button6.Text = "右转";
            this.button6.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1161, 587);
            this.Controls.Add(this.button6);
            this.Controls.Add(this.button5);
            this.Controls.Add(this.button4);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.continuousDistanceBtn);
            this.Controls.Add(this.singleDistanceBtn);
            this.Controls.Add(this.openLaserBtn);
            this.Controls.Add(this.addressComboBox);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.disconnectLaserBtn);
            this.Controls.Add(this.connectLaserBtn);
            this.Controls.Add(this.baudComboBox);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.portComboBox);
            this.Controls.Add(this.motorCloseBtn);
            this.Controls.Add(this.stopBtn);
            this.Controls.Add(this.startBtn);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.velBox);
            this.Controls.Add(this.runMode);
            this.Controls.Add(this.motorConnectBtn);
            this.Name = "Form1";
            this.Text = "Form1";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button motorConnectBtn;
        private System.Windows.Forms.CheckBox runMode;
        private System.Windows.Forms.TextBox velBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button startBtn;
        private System.Windows.Forms.Button stopBtn;
        private System.Windows.Forms.Button motorCloseBtn;
        private System.Windows.Forms.ComboBox portComboBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox baudComboBox;
        private System.Windows.Forms.Button connectLaserBtn;
        private System.Windows.Forms.Button disconnectLaserBtn;
        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox addressComboBox;
        private System.Windows.Forms.Button openLaserBtn;
        private System.Windows.Forms.Button singleDistanceBtn;
        private System.Windows.Forms.Button continuousDistanceBtn;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.Button button5;
        private System.Windows.Forms.Button button6;
    }
}

