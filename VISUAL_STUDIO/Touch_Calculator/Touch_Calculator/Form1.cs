using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Touch_Calculator
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }


        /* (225,224) (018,224)
           (226,026) (018,026) old*/

        /* (022,235) (243,234)
           (022,021) (241,016) new*/
        private void button1_Click(object sender, EventArgs e)
        {
            int a, b;
            int min, max;
            string XDirection = "";
            string YDirection = "";

            dgv.Rows.Clear();

            // определим направление координат экрана

            a = Convert.ToInt16(px_LB.Text);
            b = Convert.ToInt16(px_RB.Text);
            XDirection = (a > b ? "RL" : "LR");

            a = Convert.ToInt16(py_LB.Text);
            b = Convert.ToInt16(py_LT.Text);
            YDirection = (a < b ? "BT" : "TB");

            addRow("X Direction", "XDirection" + XDirection);
            addRow("Y Direction", "YDirection" + YDirection);

            // вычислим координаты

            a = Convert.ToInt16(px_LB.Text);
            b = Convert.ToInt16(px_LT.Text);
            min = (a > b ? b : a);
            max = (a > b ? a : b);
            int LX = (min + ((max - min) / 2));

            a = Convert.ToInt16(px_RB.Text);
            b = Convert.ToInt16(px_RT.Text);
            min = (a > b ? b : a);
            max = (a > b ? a : b);
            int RX = (min + ((max - min) / 2));

            a = Convert.ToInt16(py_LB.Text);
            b = Convert.ToInt16(py_RB.Text);
            min = (a > b ? b : a);
            max = (a > b ? a : b);
            int BY = (min + ((max - min) / 2));

            a = Convert.ToInt16(py_LT.Text);
            b = Convert.ToInt16(py_RT.Text);
            min = (a > b ? b : a);
            max = (a > b ? a : b);
            int TY = (min + ((max - min) / 2));

            addRow(".Left X", LX);
            addRow(".Right X", RX);
            addRow(".Bottom Y", BY);
            addRow(".Top Y", TY);

            int w = Math.Abs(LX - RX);
            int h = Math.Abs(BY - TY);

            addRow(".Visible W", w);
            addRow(".Visible H", h);

            // BT
            int Line1Y=0, Line2Y=0, Line3Ynet=0;

            if (YDirection == "BT")
            {
                Line1Y = (int)(BY + h * 0.208);  // нижняя линия
                Line3Ynet = (int)(h * 0.27);   // верхняя линия
                Line2Y = BY + Line1Y + (h - Line1Y - Line3Ynet); // средняя линия
            }

            addRow("Line1Y", Line1Y);
            addRow("Line2Y", Line2Y);

            int BtnCancelX = 0, BtnOkX = 0, BtnMinusX = 0, BtnPlusX = 0;

            if (XDirection == "RL")
            {
                BtnCancelX = (int)(RX + w / 4);
                BtnOkX = (int)(BtnCancelX + w / 4);
                BtnMinusX = (int)(BtnOkX + w / 4);
                BtnPlusX = (int)(BtnMinusX + w / 4);
            }
            if (XDirection == "LR")
            {
                BtnPlusX = (int)(LX + w / 4);
                BtnMinusX = (int)(BtnPlusX + w / 4);
                BtnOkX = (int)(BtnMinusX + w / 4);
                BtnCancelX = (int)(BtnOkX + w / 4);
            }

            addRow("BtnCancelX", BtnCancelX);
            addRow("BtnOkX", BtnOkX);
            addRow("BtnMinusX", BtnMinusX);
            addRow("BtnPlusX", BtnPlusX);

            int BtnMinutesX = 0, BtnHoursX = 0;

            if (XDirection == "RL")
            {
                BtnMinutesX = (int)(RX + w * 0.425);
                BtnHoursX = (int)(BtnMinutesX + w * 0.15);
            }
            if (XDirection == "LR")
            {
                BtnHoursX = (int)(LX + w * 0.425);
                BtnMinutesX = (int)(BtnHoursX + w * 0.15);
            }

            addRow("BtnMinutesX", BtnMinutesX);
            addRow("BtnHoursX", BtnHoursX);

            int BtnAlarmX = 0;

            if (XDirection == "RL")
                BtnAlarmX = (int)(RX + w/2);
            if (XDirection == "LR")
                BtnAlarmX = (int)(LX + w / 2);

            addRow("BtnAlarmX", BtnAlarmX);

            int BtnAlarmLine1 = 0, BtnAlarmLine2 = 0, BtnAlarmLine3 = 0, BtnAlarmLine4 = 0;

            if (YDirection == "BT")
            {
                BtnAlarmLine1 = (int)(TY - (h - (h * 0.208)) / 4);
                BtnAlarmLine2 = (int)(BtnAlarmLine1 - (h - (h * 0.208)) / 4);
                BtnAlarmLine3 = (int)(BtnAlarmLine2 - (h - (h * 0.208)) / 4);
                BtnAlarmLine4 = (int)(BtnAlarmLine3 - (h - (h * 0.208)) / 4);
            }

            addRow("BtnAlarmLine1", BtnAlarmLine1);
            addRow("BtnAlarmLine2", BtnAlarmLine2);
            addRow("BtnAlarmLine3", BtnAlarmLine3);
            addRow("BtnAlarmLine4", BtnAlarmLine4);
        }

        void addRow(string key, object data)
        {
            DataGridViewRow r = (DataGridViewRow)dgv.Rows[0].Clone();
            r.Cells[0].Value = key;
            r.Cells[1].Value = data;
            dgv.Rows.Add(r);
        }

        private void button2_Click(object sender, EventArgs e)
        {
            px_LT.Text = "";
            px_LB.Text = "";
            px_RT.Text = "";
            px_RB.Text = "";
            py_LT.Text = "";
            py_LB.Text = "";
            py_RT.Text = "";
            py_RB.Text = "";

            dgv.Rows.Clear();
        }

        private void button3_Click(object sender, EventArgs e)
        {
            string result = "";
            foreach (DataGridViewRow dr in dgv.Rows)
            {
                if (dr.Cells[0].Value != null)
                    if (dr.Cells[0].Value.ToString()[0] != '.')
                    {
                        if (dr.Cells[0].Value.ToString().Contains(" "))
                            result += $"#define {dr.Cells[1].Value}\r\n";
                        else
                            result += $"#define {dr.Cells[0].Value} {dr.Cells[1].Value}\r\n";
                    }
            }
            Clipboard.SetText(result);
        }
    }
}
