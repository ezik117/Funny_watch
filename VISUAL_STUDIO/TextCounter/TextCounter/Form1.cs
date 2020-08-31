using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;

namespace TextCounter
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        List<byte> L = new List<byte>();

        //-----------------------------------------------------------------------------------------
        private void btnOpenTxtFile_Click(object sender, EventArgs e)
        {
            OpenFileDialog of = new OpenFileDialog();
            of.InitialDirectory = Application.StartupPath;
            of.Filter = "TXT | *.txt";
            if (of.ShowDialog() == DialogResult.Cancel) return;

            byte[] a = new byte[1];
            string s;

            FileStream fs = File.OpenRead(of.FileName);
            int ret = fs.Read(a, 0, 1);
            int charsTotal = 0;
            
            while (ret > 0)
            {
                if ((a[0] != '\r') && (a[0] != '\n'))
                {
                    charsTotal++;

                    if (!L.Contains(a[0]))
                        L.Add(a[0]);
                }
                ret = fs.Read(a, 0, 1);
            }
            fs.Close();

            lbl1.Text = "Букв всего: " + charsTotal.ToString();
            lbl2.Text = "Из них уникальных: " + L.Count.ToString();
            lbl3.Text = "Не использовано букв: " + (32 - L.Count).ToString();

            lb.Items.Clear();
            foreach (byte b in L)
            {
                s = System.Text.Encoding.GetEncoding(1251).GetString(new byte[] {b});
                lb.Items.Add(string.Format("{0}  |  0x{1:X2}  |  POS: {2:N0}", s, b, b - 0xC0));
            }
            lb.Sorted = true;
        }
    }
}
