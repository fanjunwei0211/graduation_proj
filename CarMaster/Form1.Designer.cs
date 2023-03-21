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
            this.motorConnectBtn = new System.Windows.Forms.Button();
            this.runMode = new System.Windows.Forms.CheckBox();
            this.velBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.startBtn = new System.Windows.Forms.Button();
            this.stopBtn = new System.Windows.Forms.Button();
            this.motorCloseBtn = new System.Windows.Forms.Button();
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
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(9F, 18F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1161, 820);
            this.Controls.Add(this.motorCloseBtn);
            this.Controls.Add(this.stopBtn);
            this.Controls.Add(this.startBtn);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.velBox);
            this.Controls.Add(this.runMode);
            this.Controls.Add(this.motorConnectBtn);
            this.Name = "Form1";
            this.Text = "Form1";
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
    }
}

