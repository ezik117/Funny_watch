using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace bmp_converter
{
    public partial class frmColorConvertor : Form
    {
        public frmColorConvertor()
        {
            InitializeComponent();
        }

        //-----------------------------------------------------------------------------------------
        private void btnToILI_Click(object sender, EventArgs e)
        {
            int wRGB = Convert.ToInt32("0x" + tbWRGB.Text, 16);
            Color color888 = Color.FromArgb(((wRGB >>16)&0xFF), ((wRGB >> 8) & 0xFF), ((wRGB) & 0xFF));
            int c565 = ((color888.R & 0xF8) << 8) | ((color888.G & 0xFC) << 3) | (color888.B >> 3);
            Color color565 = Color.FromArgb((((c565 & 0xF800) >> 11) & 0xFF), (((c565 & 0x07E0) >> 5) & 0xFF), ((c565 & 0x001F) & 0xFF));

            tbWR.Text = color888.R.ToString();
            tbWG.Text = color888.G.ToString();
            tbWB.Text = color888.B.ToString();

            tbIR.Text = color565.R.ToString();
            tbIG.Text = color565.G.ToString();
            tbIB.Text = color565.B.ToString();

            tbIRGB.Text = string.Format("{0:X4}", (UInt16)(c565 & 0xFFFF));
            pnlColor.BackColor = color888;
        }

        //-----------------------------------------------------------------------------------------
        private void btnToWin_Click(object sender, EventArgs e)
        {
            int[] Table5 = {0, 8, 16, 25, 33, 41, 49, 58, 66, 74, 82, 90, 99, 107, 115, 123, 132, 140, 148, 156, 165, 173, 181, 189, 197, 206, 214, 222, 230, 239, 247, 255};
            int[] Table6 = {0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 45, 49, 53, 57, 61, 65, 69, 73, 77, 81, 85, 89, 93, 97, 101, 105, 109, 113, 117, 121, 125, 130, 134, 138, 142, 146, 150, 154, 158, 162, 166, 170, 174, 178, 182, 186, 190, 194, 198, 202, 206, 210, 215, 219, 223, 227, 231, 235, 239, 243, 247, 251, 255};

            int iRGB = Convert.ToInt32("0x" + tbIRGB.Text, 16);
            Color color565 = Color.FromArgb((((iRGB & 0xF800) >>11) & 0xFF), (((iRGB & 0x07E0) >>5) & 0xFF), ((iRGB & 0x001F) & 0xFF));
            Color color888 = Color.FromArgb(Table5[color565.R], Table6[color565.G], Table5[color565.B]);

            tbWR.Text = color888.R.ToString();
            tbWG.Text = color888.G.ToString();
            tbWB.Text = color888.B.ToString();

            tbIR.Text = color565.R.ToString();
            tbIG.Text = color565.G.ToString();
            tbIB.Text = color565.B.ToString();

            tbWRGB.Text = string.Format("{0:X2}{1:X2}{2:X2}", color888.R, color888.G, color888.B);
            pnlColor.BackColor = color888;
        }
    }
}
