using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using SDKDemo;

namespace CarMaster
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
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
    }
}
