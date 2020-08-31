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

namespace MCU_ILI9341_Picture_Editor
{
    public partial class Form1 : Form
    {


        public Form1()
        {
            InitializeComponent();
        }

        //-----------------------------------------------------------------------------------------
        private void btn_PB1_Load_Click(object sender, EventArgs e)
        {
            OpenFileDialog od = new OpenFileDialog();
            if (od.ShowDialog() == System.Windows.Forms.DialogResult.Cancel) return;

            PB1.Load(od.FileName);
            TB1.Text = "";
        }

        //-----------------------------------------------------------------------------------------
        private void btn_PB1_Convert_Click(object sender, EventArgs e)
        {
            TB1.Text = "";
            TB1.Text += "Width: " + PB1.Image.Width.ToString() + "\r\n";
            TB1.Text += "Height: " + PB1.Image.Height.ToString() + "\r\n";
            TB1.Text += "Count: " + (PB1.Image.Width * PB1.Image.Height).ToString() + "\r\n";

            Color col;
            int PixelsPerLine = 64;
            int PPLCount = 0;
            bool firstLine = true;
            string ColorStr = "";

            int X_start = 0, X_end = 0, X_step = 0, Y_start = 0, Y_end = 0, Y_step = 0;

            int px_x, px_y;

            if (rbXLR.Checked)
            {
                X_start = -1;
                X_end = PB1.Image.Width-1;
                X_step = 1;
            }
            if (rbXRL.Checked)
            {
                X_start = PB1.Image.Width-1;
                X_end = -1;
                X_step = -1;
            }
            if (rbYBT.Checked)
            {
                Y_start = PB1.Image.Height-1;
                Y_end = -1;
                Y_step = -1;
            }
            if (rbYTB.Checked)
            {
                Y_start = -1;
                Y_end = PB1.Image.Height-1;
                Y_step = 1;
            }

            using (BinaryWriter writer = new BinaryWriter(File.Open(Application.StartupPath + "\\" + tbFileName.Text + ".bin", FileMode.Create)))
            {

                for (int y = Y_start; y != Y_end; y += Y_step)
                    for (int x = X_start; x != X_end; x += X_step)
                    {
                        //начало новой строки
                        if (PPLCount++ == 0)
                            if (!cbToFileOnly.Checked) TB1.Text += "DW ";

                        //вставим заголовок
                        if (firstLine)
                        {
                            firstLine = false;
                            //ширина X
                            TB1.Text += "0x" + (PB1.Image.Width - 1).ToString("X4") + ",";
                            writer.Write((byte)((PB1.Image.Width - 1) >> 8));
                            writer.Write((byte)((PB1.Image.Width - 1)));
                            //высота Y
                            TB1.Text += "0x" + (PB1.Image.Height - 1).ToString("X4") + ",";
                            writer.Write((byte)((PB1.Image.Height - 1) >> 8));
                            writer.Write((byte)((PB1.Image.Height - 1)));
                            //количество точек в картинке
                            TB1.Text += "0x" + (PB1.Image.Width * PB1.Image.Height).ToString("X4") + ",";
                            writer.Write((byte)((PB1.Image.Width * PB1.Image.Height) >> 8));
                            writer.Write((byte)((PB1.Image.Width * PB1.Image.Height)));
                        }

                        //разбираем по точкам
                        px_x = x;
                        px_y = y;
                        if (X_step > 0) px_x++;
                        if (Y_step > 0) px_y++;

                        col = ((Bitmap)PB1.Image).GetPixel(px_x, px_y);
                        int PixCol = col.ToArgb();

                        ColorStr = color565((Byte)(PixCol >> 16),
                                                               (Byte)(PixCol >> 8),
                                                               (Byte)PixCol).ToString("X4");

                        if (!cbToFileOnly.Checked) //на экран
                        {
                            TB1.Text += "0x" + ColorStr;
                        }

                        //в файл
                        int IntColor = Int32.Parse(ColorStr, System.Globalization.NumberStyles.HexNumber);
                        writer.Write((byte)(IntColor >> 8));
                        writer.Write((byte)(IntColor));

                        if (PPLCount != 64)
                        {
                            if (!cbToFileOnly.Checked) TB1.Text += ",";
                        }
                        else
                        {
                            if (!cbToFileOnly.Checked) TB1.Text += "\r\n";
                            PPLCount = 0;
                        }

                    }
            }
            TB1.AppendText("\r\nDONE!");
        }

        //-----------------------------------------------------------------------------------------
        //Convert red, green, blue components to a 16-bit 565 RGB value. Components
        private UInt16 color565(Byte r, Byte g, Byte b)
        {
            int ret = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
            return (UInt16)ret;
        }

        //-----------------------------------------------------------------------------------------
        private void btnClear_Click(object sender, EventArgs e)
        {
            TB1.Text = "";
            PB1.Image = null;
            PB1.Image = null;
            PB1.Image = null;
            PB1.Image = null;
            PB1.Image = null;
        }

        //-----------------------------------------------------------------------------------------
        //конвертирует картинку кратную 8 битам в ширину в байтовые коды сверху вниз справа налево
        //картинка должна быть BMP, с белым фоном и черными точками
        private void btn_ConvertToByte_Click(object sender, EventArgs e)
        {
            TB1.Text = "";
            TB1.Text += "Width (px): " + PB1.Image.Width.ToString() + "\r\n";
            TB1.Text += "Height (px): " + PB1.Image.Height.ToString() + "\r\n";
            TB1.Text += "Width (bytes): " + (PB1.Image.Width / 8).ToString() + "\r\n";
            TB1.Text += "Total bytes: " + ((PB1.Image.Width / 8) * PB1.Image.Height).ToString() + "\r\n";

            if (!cbToFileOnly.Checked) TB1.Text += "DB ";
            byte buf = 0x00;
            int bits_count = 0;
            Color col;

            using (BinaryWriter writer = new BinaryWriter(File.Open(Application.StartupPath + "\\" + tbFileName.Text + ".bin", FileMode.Create)))
            {
                for (int y = 0; y < PB1.Image.Height; y++)
                {
                    for (int x = 0; x < PB1.Image.Width; x++)
                    {
                        col = ((Bitmap)PB1.Image).GetPixel(x, y);
                        if (col == Color.FromArgb(0xFF, 0x00, 0x00, 0x00)) //black
                        {
                            buf = (byte)((buf << 1) | 0x01);
                        }
                        else
                        {
                            buf = (byte)(buf << 1);
                        }
                        bits_count++;
                        if (bits_count == 8)
                        {
                            bits_count = 0;
                            if (!cbToFileOnly.Checked) TB1.Text += "0x" + buf.ToString("X2") + ",";
                            writer.Write((byte)(buf));

                            buf = 0;
                        }
                    }
                }
            }
            if (!cbToFileOnly.Checked) TB1.Text = TB1.Text.Remove(TB1.Text.Length - 1, 1);
            TB1.AppendText("\r\nDONE!");
        }
	



    }
}
