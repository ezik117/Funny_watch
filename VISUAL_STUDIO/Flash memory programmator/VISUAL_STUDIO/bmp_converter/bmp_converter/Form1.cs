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

namespace bmp_converter
{
    public partial class Form1 : Form
    {
        List<Color> usedColors = new List<Color>();
        Color[] colorTable = new Color[256];
        struct TBmpHeader
        {
            public uint HeaderLen;
            public uint BPP;
            public uint width;
            public uint height;
            public uint dataOffset;
            public uint colorTable;
            public uint colorCount; //в ячейках, не в байтах
            public uint imageSize; 
        } ;
        TBmpHeader bm = new TBmpHeader();
        string tmp = string.Empty;

        public Form1()
        {
            InitializeComponent(); 
        }

        //-----------------------------------------------------------------------------------------
        /* ЗАГРУЗИТЬ КАРТИНКУ */
        private void btnLoad_Click(object sender, EventArgs e)
        {
            //диалог открытия файла
            OpenFileDialog of = new OpenFileDialog();
            of.InitialDirectory = Application.StartupPath;
            of.Filter = "*.bmp|*.bmp";
            if (of.ShowDialog() == DialogResult.Cancel) return;

            //очистка дисплея
            tbInfo.Clear();
            while (splitContainer1.Panel2.Controls.Count > 0)
            {
                ((TextBox)splitContainer1.Panel2.Controls[0]).Dispose();
            }

            //загружаем
            pic.Image = null;
            pic.Load(of.FileName);

            tbInfo.AppendText("Load image: " + of.FileName + "\r\n");
            tbInfo.AppendText(String.Format("Image loaded. Width:{0:N0}px. Height:{1:N0}px.\r\n", pic.Image.Width, pic.Image.Height));

            btnSaveImage.Enabled = true;
        }

        //-----------------------------------------------------------------------------------------
        private void old_btnLoad_Click(object sender, EventArgs e)
        {
            OpenFileDialog of = new OpenFileDialog();
            of.InitialDirectory = Application.StartupPath;
            of.Filter = "*.bmp|*.bmp";
            if (of.ShowDialog() == DialogResult.Cancel) return;

            tbInfo.Clear();
            while (splitContainer1.Panel2.Controls.Count > 0)
            {
                ((TextBox)splitContainer1.Panel2.Controls[0]).Dispose();
            }
            FileStream fs = new FileStream(of.FileName, FileMode.Open);

            //заполняем данные из таблицы
            bm.dataOffset = (uint)ReadData(ref fs, 10, 4);
            bm.HeaderLen = (uint)ReadData(ref fs, 14, 4);
            bm.width = (uint)ReadData(ref fs, 18, 4);
            bm.height = (uint)ReadData(ref fs, 22, 4);
            bm.BPP = (uint)ReadData(ref fs, 28, 2);
            bm.colorTable = 54;
            bm.colorCount = (uint)ReadData(ref fs, 46, 4);
            if (bm.colorCount == 0)//full color table
            {
                if (bm.BPP == 8) bm.colorCount = 256; 
                if (bm.BPP == 4) bm.colorCount = 16;
            }
            bm.imageSize = (uint)ReadData(ref fs, 34, 4);

            if (bm.HeaderLen != 40)
            {
                MessageBox.Show("BMP header (BITMAPINFOHEADER) must be version 3.", "error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                fs.Close();
                return;
                
            }

            byte[] buf = new byte[4];
            usedColors.Clear();

            //считаем префикс BM
            ClearBuf(ref buf);
            fs.Position = 0;
            fs.Read(buf, 0, 2);
            if (System.Text.Encoding.Default.GetString(buf, 0, 2) == "BM")
            {
                //считаем в память палитру
                if ((bm.BPP == 8) || (bm.BPP == 4))
                {
                    int idx = 0;
                    fs.Position = bm.colorTable;
                    for (int i=0; i<bm.colorCount; i++)
                    {
                        fs.Read(buf, 0, 4);
                        colorTable[idx++] = Color.FromArgb(buf[2], buf[1], buf[0]);
                    }

                    //создадим растровую картинку
                    pic.Width = (int)bm.width;
                    pic.Height = (int)bm.height;
                    pic.Image = new Bitmap((int)bm.width, (int)bm.height);



                    //заполним картинку    
                    //256 цветов
                    if (bm.BPP == 8)
                    {
                        //определим размер ряда
                        int xdiff = pic.Width - ((int)(pic.Width / 4) * 4);

                        fs.Position = bm.dataOffset;
                        for (int h = (pic.Height-1); h >= 0 ; h--)
                            for (int w = 0; w < pic.Width; w++)
                            {
                                fs.Read(buf, 0, 1);
                                idx = buf[0];
                                ((Bitmap)pic.Image).SetPixel(w, h, colorTable[idx]);

                                if (!usedColors.Contains(colorTable[idx])) usedColors.Add(colorTable[idx]);
                            }
                    }
                    //16 цветов
                    int ax, ay;
                    if (bm.BPP == 4)
                    {
                        //определим размер ряда
                        int xdiff = (pic.Width / 2) - (int)((int)(pic.Width / 2 / 4) * 4);

                        fs.Position = bm.dataOffset;
                        for (int h = (pic.Height - 1); h >= 0; h--)
                        {
                            ax = 0;
                            for (int w = 0; w < ((pic.Width & 1) == 1 ? ((pic.Width + 1) / 2) : (pic.Width / 2)); w++)
                            {
                                fs.Read(buf, 0, 1);
                                ((Bitmap)pic.Image).SetPixel(ax, h, colorTable[(buf[0] >> 4)]);
                                ax++;
                                ((Bitmap)pic.Image).SetPixel(ax, h, colorTable[(buf[0] & 0x0F)]);
                                ax++;
                                if (!usedColors.Contains(colorTable[(buf[0] >> 4)])) usedColors.Add(colorTable[(buf[0] >> 4)]);
                                if (!usedColors.Contains(colorTable[(buf[0] & 0x0F)])) usedColors.Add(colorTable[(buf[0] & 0x0F)]);

                            }
                        }
                    }

                    //выведем палитру использованных в картинке цветов
                    int x = 0;
                    int y = 0;
                    foreach (Color c in usedColors)
                    {
                        TextBox p = new TextBox();
                        p.Width = 50;
                        p.Height = 17;
                        p.BackColor = c;
                        p.Left = x;
                        p.Top = y;
                        x += 50;
                        p.TextAlign = HorizontalAlignment.Center;
                        p.ForeColor = Color.FromArgb(0xAA ^ c.R, 0xAA ^ c.G, 0xAA ^ c.B);
                        p.Text = String.Format("{0:X2}{1:X2}{2:X2}", c.R, c.G, c.B);
                        
                        p.Parent = splitContainer1.Panel2;

                        if (x > (splitContainer1.Panel2.Width - 50))
                        {
                            x = 0;
                            y += 17;
                        }
                    }
                    btnSaveImage.Enabled = true;

                    //показываем всяку инфу
                    tbInfo.AppendText(String.Format("Color bits per pixel: {0:N0}. Used colors: {1:N0}\r\n", bm.BPP, usedColors.Count));
                    tbInfo.AppendText(String.Format("Dimension: W({0:N0}) x H({1:N0}). Pixels: {2:N0}. Bytes: {3:N0}\r\n", bm.width, bm.height, (bm.width*bm.height), bm.imageSize));
                }
                else
                {
                    MessageBox.Show("Image is neither in 256 or 16 colors mode!", "error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            else
            {
                MessageBox.Show("It is not BMP file!", "error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            fs.Close();
        }

        //-----------------------------------------------------------------------------------------
        /* считывает указанное кол-во байт из файла и преобразует в long */
        long ReadData(ref FileStream fs, long position, int bytes)
        {
            byte[] buf = new byte[4];
            fs.Position = position;
            fs.Read(buf, 0, bytes);
            long ret = 0;
            for (int pow = 0; pow < bytes; pow++)
            {
                ret += Convert.ToInt64(Math.Pow(2, pow*8) * buf[pow]);
            }
            return ret;
        }

        //-----------------------------------------------------------------------------------------
        void ClearBuf(ref byte[] buf)
        {
            for (int i = 0; i < 4; i++) buf[i] = 0;
        }

        //-----------------------------------------------------------------------------------------
        /* СОХРАНИТЬ ДАННЫЕ */
        private void btnSaveImage_Click(object sender, EventArgs e)
        {
            int rx, ry;
            int dx, dy;
            Color px;
            int _px;
            byte idx;
            byte c4bpp = 0;
            int datalen = 0;

            List<Color> cm = new List<Color>();
            cm.Clear();

            //анализ палитры (16)
            if (rb16ColorDots.Checked)
            {
                for (int y = 0; y < pic.Image.Height; y++)
                    for (int x = 0; x < pic.Image.Width; x++)
                    {
                        px = ((Bitmap)pic.Image).GetPixel(x, y);
                        if (!cm.Contains(px)) cm.Add(px);
                    }
            }
            if (cm.Count > 16)
            {
                MessageBox.Show("Image contains more than 16 colors! Processing stops. Please save image in 4-bit mode.", "error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            while (cm.Count < 16) cm.Add(Color.Black); //добьем до 16

            //анализ палитры (256)
            if (rb256ColorDots.Checked)
            {
                for (int y = 0; y < pic.Image.Height; y++)
                    for (int x = 0; x < pic.Image.Width; x++)
                    {
                        px = ((Bitmap)pic.Image).GetPixel(x, y);
                        if (!cm.Contains(px)) cm.Add(px);
                    }
            }
            if (cm.Count > 256)
            {
                MessageBox.Show("Image contains more than 256 colors! Processing stops. Please save image in 8-bit mode.", "error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            //порядок обработки точек
            rx = (rbLeftToRight.Checked ? 0 : pic.Image.Width - 1);
            ry = (rbTopToBottom.Checked ? 0 : pic.Image.Height - 1);
            dx = (rbLeftToRight.Checked ? +1 : -1);
            dy = (rbTopToBottom.Checked ? +1 : -1);

            //диалог выбора файла
            SaveFileDialog sf = new SaveFileDialog();
            sf.InitialDirectory = Application.StartupPath;
            sf.OverwritePrompt = !cbAddToExisted.Checked;
            if (sf.ShowDialog() == DialogResult.Cancel) return;

            //открываем файл
            FileStream fs = new FileStream(sf.FileName, (cbAddToExisted.Checked ? FileMode.OpenOrCreate : FileMode.Create));
            if (cbAddToExisted.Checked)
            {
                fs.Position = fs.Length;
                tmp = "x";
            }
            else
            {
                tmp = string.Empty;
            }
                
            //сохраняем заголовок
            //width, height
            SaveByte(ref fs, (byte)pic.Image.Width);
            SaveByte(ref fs, (byte)(pic.Image.Width >> 8));
            SaveByte(ref fs, (byte)pic.Image.Height);
            SaveByte(ref fs, (byte)(pic.Image.Height >> 8));
            //data length
            if (rbFullColor.Checked)
            {
                datalen = pic.Image.Width * pic.Image.Height * 2;
            }
            //
            if (rb16ColorDots.Checked)
            {
                int h = pic.Image.Height;
                if ((h & 0x01) == 0x01) h++;
                datalen = (pic.Image.Width * h) / 2;
            }
            //
            if (rb256ColorDots.Checked)
            {
                datalen = pic.Image.Width * pic.Image.Height;
                SaveByte(ref fs, (byte)(datalen));
                SaveByte(ref fs, (byte)(datalen >> 8));             
                //color matrix length - только для 8и битного цвета
                datalen = cm.Count * 2;
            }
            //
            SaveByte(ref fs, (byte)(datalen));
            SaveByte(ref fs, (byte)(datalen >> 8));
            SaveByte(ref fs, (byte)(datalen >> 16));
            SaveByte(ref fs, (byte)(datalen >> 24));

            //color matrix - только для 4х и 8и битного цвета
            if (rb16ColorDots.Checked || rb256ColorDots.Checked)
            {
                foreach(Color c in cm)
                {
                    _px = color565(c);
                    SaveByte(ref fs, (byte)(_px));
                    SaveByte(ref fs, (byte)(_px >> 8));                  
                }
            }

            //обработка файла
            tbInfo.AppendText("Writing to file " + sf.FileName + "\r\n");

            for (int y = 0; y < pic.Image.Height; y++)
            {
                rx = (rbLeftToRight.Checked ? 0 : pic.Image.Width - 1);
                for (int x = 0; x < pic.Image.Width; x++)
                {
                    px = ((Bitmap)pic.Image).GetPixel(rx, ry);

                    //сохранить в полноцветном формате
                    if (rbFullColor.Checked)
                    {
                        _px = color565(px);
                        SaveByte(ref fs, (byte)(_px >> 8));
                        SaveByte(ref fs, (byte)(_px));                                          
                    }

                    //сохранить в 4 битном формате (16 цветов)
                    if (rb16ColorDots.Checked)
                    {
                        idx = findColorIdx(ref cm, px);

                        if ((x & 0x01) == 0x00)
                        {
                            c4bpp = (byte)(idx << 4);
                            if (x == (pic.Image.Width-1)) //последняя точка по ширине попала на первую точку в байте
                            {
                                SaveByte(ref fs, c4bpp);
                            }
                        }
                        else
                        {
                            c4bpp |= idx;
                            SaveByte(ref fs, c4bpp);
                        }
                    }

                    //сохранить в черно белом формате
                    if (rbBlackWhiteDots.Checked)
                    {

                    }

                    rx += dx; 
                }
                ry += dy;
            }

            //finish 1
            if (rbFullColor.Checked)
            {
                tbInfo.AppendText(string.Format("Done. Length: {0:N0} bytes.\r\n", 8 + (pic.Image.Width * pic.Image.Height * 2)));
            }
            //finish 2
            if (rb16ColorDots.Checked)
            {
                int h = pic.Image.Height;
                if ((h & 0x01) == 0x01) h++;
                datalen = (pic.Image.Width * h) / 2;
                tbInfo.AppendText(string.Format("Done. Length: {0:N0} bytes.\r\n", 36 + datalen));
            }

            fs.Close();

            //int rx, ry;
            //UInt16 px;

            //if (rb256ColorMatrix.Checked)
            //{
            //    //вывод палитры
            //    //формат палитры: 1ый байт - кол-во цветов в палитре. далее по 3 байта на цвет.
            //    FileStream fs;
            //    if (cbAddToExisted.Checked)
            //    {
            //        if (!rbBinaryData.Checked)
            //        {
            //            MessageBox.Show("Output must be set as binary data!");
            //            return;
            //        }
            //        fs = new FileStream(Application.StartupPath + @"\colorMatrix.dat", FileMode.OpenOrCreate);
            //        if (fs.Length == 0) for (int i = 0; i < 257; i++) fs.WriteByte(0x00);
            //        fs.Position = 0;
            //    }
            //    else
            //    {
            //        fs = new FileStream(Application.StartupPath + @"\colorMatrix.dat", FileMode.Create);
            //        fs.Position = 0;
            //    }

            //    int colCount;

            //    if (cbAddToExisted.Checked)
            //    {
            //        colCount = fs.ReadByte();
            //    }
            //    else
            //    {
            //        colCount = usedColors.Count;
            //        fs.WriteByte((byte)colCount);
            //        for (int i=0; i<usedColors.Count; i++)
            //        {
            //            //px = usedColors[i]
            //        }
            //        for (int i=0; i<(256-usedColors.Count); i++)
            //        {
            //            fs.WriteByte(0x00);
            //            fs.WriteByte(0x00);
            //        }
            //    }


            //        fs.Close();
            //}
            //else
            //{
            //    FileStream fs = new FileStream(Application.StartupPath + @"\converted_bmp.dat", FileMode.Create);
            //    //вывод изображения
            //    for (int x = 0; x < pic.Image.Width; x++)
            //        for (int y = 0; y < pic.Image.Height; y++)
            //        {
            //            rx = (rbLeftToRight.Checked ? x : pic.Image.Width - x - 1);
            //            ry = (rbTopToBottom.Checked ? y : pic.Image.Height - y - 1);

            //            //черно-белые точки в побитовом режиме
            //            if (rbBlackWhiteDots.Checked)
            //            {
            //                if (rbBinaryData.Checked)
            //                {

            //                }
            //                else
            //                {

            //                }
            //            }



            //            //((Bitmap)pic.Image).SetPixel(rx, ry, Color.Red);
            //        }
            //    fs.Close();
            //}


        }

        //-----------------------------------------------------------------------------------------
        //Convert red, green, blue components to a 16-bit 565 RGB value (2 bytes)
        private UInt16 color565(Color color)
        {
            int ret = ((color.R & 0xF8) << 8) | ((color.G & 0xFC) << 3) | (color.B >> 3);
            return (UInt16)(ret & 0xFFFF);
        }

        private UInt16 color565(Byte r, Byte g, Byte b)
        {
            int ret = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
            return (UInt16)(ret & 0xFFFF);
        }

        //-----------------------------------------------------------------------------------------
        /* Сохранение в файл */
        void SaveByte(ref FileStream fs, byte b)
        {
            if (rbBinaryData.Checked)
            {
                fs.WriteByte(b);
            }
            else
            {
                if (tmp == string.Empty)
                {
                    tmp = string.Format("0x{0:X2}", b);
                }
                else
                {
                    tmp = string.Format(",0x{0:X2}", b);
                }
                byte[] buf = Encoding.ASCII.GetBytes(tmp);
                for (int i = 0; i < tmp.Length; i++)
                    fs.WriteByte(buf[i]);
            }
        }

        //-----------------------------------------------------------------------------------------
        /* найти индекс по цвету */
        byte findColorIdx(ref List<Color> lc, Color c)
        {
            byte res = 0;
            foreach (Color col in lc)
            {
                if (col == c)
                {
                    break;
                }
                else
                {
                    res++;
                }
            }
            return res;
        }

        //-----------------------------------------------------------------------------------------
        private void mnuColorConvertor_Click(object sender, EventArgs e)
        {
            frmColorConvertor frm = new frmColorConvertor();
            frm.ShowDialog();
        }
    }
}
