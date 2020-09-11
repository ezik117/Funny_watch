using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TextConverter
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            string s0 = tbSrc.Text;
            Encoding utf8 = Encoding.GetEncoding("UTF-8");
            Encoding win1251 = Encoding.GetEncoding("Windows-1251");

            byte[] a = utf8.GetBytes(s0);
            byte[] b = Encoding.Convert(utf8, win1251, a);

            string s1 = "\"";
            foreach (byte d in b)
            {
                s1 += "\\x" + d.ToString("X2"); 
            }

            tbDst.Text = s1 + "\"; // " + s0;
        }

        private void tbSrc_TextChanged(object sender, EventArgs e)
        {
            string s0 = tbSrc.Text;
            Encoding utf8 = Encoding.GetEncoding("UTF-8");
            Encoding win1251 = Encoding.GetEncoding("Windows-1251");

            byte[] a = utf8.GetBytes(s0);
            byte[] b = Encoding.Convert(utf8, win1251, a);

            string s1 = "\"";
            foreach (byte d in b)
            {
                s1 += "\\x" + d.ToString("X2");
            }

            tbDst.Text = s1 + "\\0\" /* " + s0 + " */ \\";
        }

        private void btnClear_Click(object sender, EventArgs e)
        {
            tbSrc.Text = "";
        }

        private void btnCopy_Click(object sender, EventArgs e)
        {
            Clipboard.SetText(tbDst.Text);
        }
    }
}
