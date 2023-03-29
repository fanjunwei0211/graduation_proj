using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO.Ports;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using SDKDemo;

namespace CarMaster
{
    public partial class Form1 : Form
    {
        /// <summary>
        /// HWT101支持的波特率
        /// </summary>
        private List<int> SupportBaudRateList { get; set; } = new List<int>() {  4800, 9600, 19200, 38400, 57600, 115200, 230400 };
        private List<int> AddressList { get; set; } = new List<int>() { 4, 5, 6};
        public bool EnableRefreshDataTh { get; private set; }
        private int[] Distance = new int[3];
        public Form1()
        {
            InitializeComponent();
        }
        public static int CRC16_Work( byte[] CRC_Buf)
        {
            int i, j; int CRC_Sumx;
            CRC_Sumx = 0xFFFF; 
            for (i = 0; i < CRC_Buf.Length; i++)
            {
                CRC_Sumx ^= CRC_Buf[i]; 
                for (j = 0; j < 8; j++)
                {
                    if (!((CRC_Sumx & 0x01) == 0))
                    { CRC_Sumx >>= 1; CRC_Sumx ^= 0xA001; }
                    else { CRC_Sumx >>= 1; }
                }
            }
            return (CRC_Sumx);
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            portComboBox_MouseDown(null, null);

            // 加载激光测距波特率下拉框
            for (int i = 0; i < SupportBaudRateList.Count; i++)
            {
                baudComboBox.Items.Add(SupportBaudRateList[i]);
            }
            // 默认激光测距选中9600
            baudComboBox.SelectedItem = 9600;
            //加载激光设备地址下拉框
            for (int i = 0; i < AddressList.Count; i++)
            {
                addressComboBox.Items.Add(AddressList[i]);
            }
            //默认选中4号
            addressComboBox.SelectedItem = 4;
            //默认打开激光按钮失能
            openLaserBtn.Enabled = false;
            Thread thread = new Thread(new ThreadStart(RefreshDataTh));
            EnableRefreshDataTh = false;
            thread.Start();
        }

        private void runMode_CheckedChanged(object sender, EventArgs e)
        {
            if (runMode.Checked)
            {
                //三号电机DO1低电平 DO0高电平 0x10000->01 0000 0000 0000 0000
                NiMotionSDK.NiM_writeParam(3, 0x37, 4, 0x10000);
                NiMotionSDK.NiM_powerOff(1);
                NiMotionSDK.NiM_powerOff(2);
                NiMotionSDK.NiM_powerOn(3);
                NiMotionSDK.NiM_powerOn(4);
            }
            else
            {
                //三号电机DO1高电平 DO0低电平 0x10000->10 0000 0000 0000 0000
                NiMotionSDK.NiM_writeParam(3, 0x37, 4, 0x20000);
                NiMotionSDK.NiM_powerOn(1);
                NiMotionSDK.NiM_powerOn(2);
                NiMotionSDK.NiM_powerOff(3);
                NiMotionSDK.NiM_powerOff(4);
            }

        }

        private void motorConnectBtn_Click(object sender, EventArgs e)
        {
            int nMotorAddr = 3; //Motor Addr
            string StrConnectMsg = "{\"DeviceName\": \"COM4\", \"Baudrate\": 115200, \"Parity\": \"None\", \"DataBits\": 8, \"StopBits\": 1}";

            try
            {
                int r = NiMotionSDK.NiM_openDevice(0, StrConnectMsg);

                if (r != 0)
                {
                    MessageBox.Show("Master Open Failed", "提示");
                    return;
                }
                else
                {
                    MessageBox.Show("Master Open Succes", "提示");
                }

            }
            catch (Exception ex)
            {
                MessageBox.Show("Master Open Failed" + ex.Message, "提示");
            }
            //设置速度模式
            int rc = NiMotionSDK.NiM_changeWorkMode(nMotorAddr, NiMotionSDK.WORK_MODE.VELOCITY_MODE);
            //设置最大速度和加速度
            rc = NiMotionSDK.NiM_writeParam(nMotorAddr, 0x5B, 4, 600);
            rc = NiMotionSDK.NiM_writeParam(nMotorAddr, 0x5F, 4, 2000);
        }

        private void startBtn_Click(object sender, EventArgs e)
        {
            int speed = int.Parse(velBox.Text);
            if (runMode.Checked == true)
            {
                NiMotionSDK.NiM_moveVelocity(3, speed); //电机正转
                NiMotionSDK.NiM_moveVelocity(4, -speed);	//电机正转
            }
            else
            {
                NiMotionSDK.NiM_moveVelocity(1, speed); //电机正转
                NiMotionSDK.NiM_moveVelocity(2, -speed);	//电机正转
            }
            
            
        }

        private void motorCloseBtn_Click(object sender, EventArgs e)
        {
            NiMotionSDK.NiM_stop(0);
            NiMotionSDK.NiM_powerOff(0);
            Thread.Sleep(100);
            NiMotionSDK.NiM_closeDevice();
        }

        private void stopBtn_Click(object sender, EventArgs e)
        {
            NiMotionSDK.NiM_stop(0);
            NiMotionSDK.NiM_powerOff(0);
        }

        private void portComboBox_MouseDown(object sender, MouseEventArgs e)
        {
            portComboBox.Items.Clear();
            string[] portNameList = SerialPort.GetPortNames();

            for (int i = 0; i < portNameList.Length; i++)
            {
                portComboBox.Items.Add(portNameList[i]);
            }
        }

        private void connectLaserBtn_Click(object sender, EventArgs e)
        {
            // 获得连接的串口号和波特率
            string portName;
            int baudrate;
            try
            {
                portName = (string)portComboBox.SelectedItem;
                baudrate = (int)baudComboBox.SelectedItem;
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
                textBox1.AppendText("连接激光失败\r\n");
                return;
            }
            
            serialPort1.PortName = portName;
            serialPort1.BaudRate = baudrate;
            serialPort1.DataBits = 8;
            serialPort1.StopBits = StopBits.One;
            serialPort1.Parity = Parity.None;
            serialPort1.Open();
            openLaserBtn.Enabled = true;
            textBox1.AppendText("激光已连接\r\n");
        }

        private void disconnectLaserBtn_Click(object sender, EventArgs e)
        {
            textBox1.AppendText("激光已断开\r\n");
            serialPort1.Close();
            openLaserBtn.Enabled = false;
        }

        private void openLaserBtn_Click(object sender, EventArgs e)
        {
            Byte[] buffer = new Byte[8];
            Byte[] tbuffer = new Byte[6];
            if (openLaserBtn.Text == "打开激光")
            {
                openLaserBtn.Text = "关闭激光";
                buffer[0] = (byte)(int)addressComboBox.SelectedItem;
                buffer[1] = 0x03;
                buffer[2] = 0x00;
                buffer[3] = 0x02;
                buffer[4] = 0x00;
                buffer[5] = 0x01;
                for (int i = 0; i < tbuffer.Length; i++)
                {
                    tbuffer[i] = buffer[i];
                }
                buffer[6] = (byte)(CRC16_Work(tbuffer) & 0xff);
                buffer[7] = (byte)((CRC16_Work(tbuffer) >> 8) & 0xff);
                serialPort1.Write(buffer,0,8);
                ////打印数据到文本框
                //foreach (byte data in buffer)
                //{
                //    String str = Convert.ToString(data, 16).ToUpper();
                //    textBox1.AppendText(str.Length == 1 ? "0" + str : str);
                //}
                //textBox1.AppendText("\r\n");
            }
            else
            {
                openLaserBtn.Text = "打开激光";
                buffer[0] = (byte)(int)addressComboBox.SelectedItem;
                buffer[1] = 0x03;
                buffer[2] = 0x00;
                buffer[3] = 0x02;
                buffer[4] = 0x00;
                buffer[5] = 0x00;
                for (int i = 0; i < tbuffer.Length; i++)
                {
                    tbuffer[i] = buffer[i];
                }
                buffer[6] = (byte)(CRC16_Work(tbuffer) & 0xff);
                buffer[7] = (byte)((CRC16_Work(tbuffer) >> 8) & 0xff);
                serialPort1.Write(buffer, 0, 8);
                ////打印数据到文本框
                //foreach (byte data in buffer)
                //{
                //    String str = Convert.ToString(data, 16).ToUpper();
                //    textBox1.AppendText(str.Length == 1 ? "0" + str : str);
                //}
                //textBox1.AppendText("\r\n");
            }
        }

        private void singleDistanceBtn_Click(object sender, EventArgs e)
        {
            Byte[] buffer = new Byte[8];
            Byte[] tbuffer = new Byte[6];
            buffer[0] = (byte)(int)addressComboBox.SelectedItem; //地址位
            buffer[1] = 0x03;//功能码
            buffer[2] = 0x00;//起始地址
            buffer[3] = 0x01;
            buffer[4] = 0x00;//寄存器数量
            buffer[5] = 0x01;
            for (int i = 0; i < tbuffer.Length; i++)
            {
                tbuffer[i] = buffer[i];
            }
            buffer[6] = (byte)(CRC16_Work(tbuffer) & 0xff);
            buffer[7] = (byte)((CRC16_Work(tbuffer) >> 8) & 0xff);
            serialPort1.Write(buffer, 0, 8);
            EnableRefreshDataTh = false;
            String str = $"Dis1 = {Distance[0]}mm\r\nDis2 = {Distance[1]}mm\r\nDis3 = {Distance[2]}mm\r\n";
            textBox1.AppendText(str);
            ////打印数据到文本框
            //foreach (byte data in buffer)
            //{
            //    String str = Convert.ToString(data, 16).ToUpper();
            //    textBox1.AppendText(str.Length == 1 ? "0" + str : str);
            //}
            //textBox1.AppendText("\r\n");
        }
        /// <summary>
        /// 刷新数据线程
        /// </summary>
        private void RefreshDataTh()
        {
            while (EnableRefreshDataTh)
            {
                Thread.Sleep(100);
                if (serialPort1.IsOpen)
                {
                    textBox1.Invoke(new Action(() =>
                    {
                        String str = $"Dis1 = {Distance[0]}mm\r\nDis2 = {Distance[1]}mm\r\nDis3 = {Distance[2]}mm\r\n";
                        textBox1.AppendText(str);
                    }));
                }
            }
        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            byte[] data = new byte[serialPort1.BytesToRead];                                //定义缓冲区，因为串口事件触发时有可能收到不止一个字节
            serialPort1.Read(data, 0, data.Length);
            //打印测试
            //foreach (byte Member in data)                                                   //遍历用法
            //{
            //    string str = Convert.ToString(Member, 16).ToUpper();
            //    this.Invoke(new Action(() =>
            //    {
            //        textBox1.AppendText((str.Length == 1 ? "0" + str : str) + " ");
            //    }));

            //}
            //this.Invoke(new Action(() =>
            //{
            //    textBox1.AppendText("\r\n");
            //}));

            switch (data[0])
            {
                case 0x04:
                    Distance[0] = (int)(data[4] << 8) + (int)data[5]; break;
                case 0x05:
                    Distance[1] = (int)(data[4] << 8) + (int)data[5]; break;
                case 0x06:
                    Distance[2] = (int)(data[4] << 8) + (int)data[5]; break;

                default: break;
            }

        }

        /// <summary>
        /// 连续测距函数，实时刷新距离信息
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void continuousDistanceBtn_Click(object sender, EventArgs e)
        {
            
            Byte[] buffer = new Byte[8];
            Byte[] tbuffer = new Byte[6];
            buffer[0] = (byte)(int)addressComboBox.SelectedItem; //地址位
            buffer[1] = 0x03;//功能码
            buffer[2] = 0x00;//起始地址
            buffer[3] = 0x03;
            buffer[4] = 0x00;//寄存器数量
            buffer[5] = 0x01;
            for (int i = 0; i < tbuffer.Length; i++)
            {
                tbuffer[i] = buffer[i];
            }
            buffer[6] = (byte)(CRC16_Work(tbuffer) & 0xff);
            buffer[7] = (byte)((CRC16_Work(tbuffer) >> 8) & 0xff);
            serialPort1.Write(buffer, 0, 8);
            //开启实时刷新线程
            if (EnableRefreshDataTh)
            {
                EnableRefreshDataTh = false;
            }
            else
            {
                EnableRefreshDataTh = true;
                Thread thread = new Thread(new ThreadStart(RefreshDataTh));
                thread.Start();
            }
        }
    }
}
