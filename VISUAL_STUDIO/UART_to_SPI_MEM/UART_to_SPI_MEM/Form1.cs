using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;
using System.IO;

namespace UART_to_SPI_MEM
{
    public partial class Form1 : Form
    {
        enum SPIMEM : byte
        {
            MEM_DEV_ID = 0x90,
            MEM_READ_DATA = 0x03,
            MEM_WRITE_ENABLE = 0x06,
            MEM_WRITE_DISABLE = 0x04,
            MEM_READ_STAT_REG1 = 0x05,
            MEM_READ_STAT_REG2 = 0x35,
            MEM_SEC_ERASE_4K = 0x20,
            MEM_CHIP_ERASE_1 = 0xC7,
            MEM_CHIP_ERASE_2 = 0x60,
            MEM_PAGE_PROGRAM = 0x02

        }

        public Form1()
        {
            InitializeComponent();
            cbPort.SelectedIndex = 7;
        }


        //-----------------------------------------------------------------------------------------
        /* ОТПРАВИТЬ БАЙТ ДАННЫХ НА MCU */
        int sendByte(ref SerialPort p, byte data, ref bool IoError)
        {
            int ret;
            IoError = false;

            //отправим байт
            try
            {
                p.Write(new byte[] { data }, 0, 1);
            }
            catch (Exception ex)
            {
                stat.AppendText("[ERROR]: " + ex.Message + "\r\n");
                IoError = true;
                return 0;
            }

            //дождемся подтверждения отправки
            try
            {
                ret = p.ReadByte();
            }
            catch (Exception ex)
            {
                stat.AppendText("[ERROR]: " + ex.Message + "\r\n");
                IoError = true;
                return 0;
            }

            Thread.Sleep(5);
            return ret;
        }


        //-----------------------------------------------------------------------------------------
        /* СЧИТАТЬ ДАННЫЕ ИЗ ПАМЯТИ НА ЭКРАН */
        private void btnReadMemoryToText_Click(object sender, EventArgs e)
        {
            btnReadManufID.PerformClick();

            stat.AppendText("Reading ...\r\n");

            int AFrom = Int32.Parse(tbAR1.Text, System.Globalization.NumberStyles.HexNumber);
            int ATo = Int32.Parse(tbAR2.Text, System.Globalization.NumberStyles.HexNumber);

            SerialPort p = new SerialPort(cbPort.Text, Convert.ToInt32(tbSerSpeed.Text), Parity.None, 8, StopBits.One);
            p.Handshake = Handshake.None;
            p.RtsEnable = false;
            p.ReadTimeout = 1000;
            p.WriteTimeout = 1000;

            try
            {
                p.Open();
            }
            catch (Exception ex)
            {
                stat.AppendText("[ERROR]: " + ex.Message + "\r\n\r\n");
                return;
            }

            bool IoError = true;
            int retVal;

            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)4, ref IoError);
            retVal = sendByte(ref p, (byte)0xFF, ref IoError);
            retVal = sendByte(ref p, (byte)SPIMEM.MEM_READ_DATA, ref IoError);
            retVal = sendByte(ref p, (byte)(AFrom >> 16), ref IoError);
            retVal = sendByte(ref p, (byte)(AFrom >> 8), ref IoError);
            retVal = sendByte(ref p, (byte)(AFrom), ref IoError);

            pb.Value = 0;
            pb.Minimum = 0;
            pb.Maximum = ATo - AFrom + 1;
            
            int row = 0;

            for (int i = AFrom; i <= ATo; i++)
            {
                retVal = sendByte(ref p, (byte)0x80, ref IoError);
                if (row == 0)
                {
                    stat.AppendText(string.Format("{0:X4}  {1:X2}", i, retVal));
                }
                else
                {
                    stat.AppendText(string.Format(" {0:X2}", retVal));
                }

                row++;
                if (row == 8)
                {
                    row = 0;
                    stat.AppendText("\r\n");
                }
                pb.Value++;
                Application.DoEvents();

            }
            retVal = sendByte(ref p, (byte)0xFF, ref IoError);

            stat.AppendText("[DONE]\r\n\r\n");
            p.Close();
        }

        //-----------------------------------------------------------------------------------------
        /* СЧИТАТЬ КОД ПРОИЗВОДИТЕЛЯ И УСТРОЙСТВА */
        private void btnReadManufID_Click(object sender, EventArgs e)
        {

            SerialPort p = new SerialPort(cbPort.Text, Convert.ToInt32(tbSerSpeed.Text), Parity.None, 8, StopBits.One);
            p.Handshake = Handshake.None;
            p.RtsEnable = false;
            p.ReadTimeout = 1000;
            p.WriteTimeout = 1000;

            try
            {
                p.Open();
            }
            catch (Exception ex)
            {
                stat.AppendText("[ERROR]: " + ex.Message + "\r\n\r\n");
                return;
            }

            int ManufacturerID;
            int DeviceID;
            bool IoError = true;
            int retVal;

            try
            {
                retVal = sendByte(ref p, (byte)0, ref IoError);
                retVal = sendByte(ref p, (byte)4, ref IoError);
                retVal = sendByte(ref p, (byte)2, ref IoError);
                retVal = sendByte(ref p, (byte)SPIMEM.MEM_DEV_ID, ref IoError);
                retVal = sendByte(ref p, (byte)0, ref IoError);
                retVal = sendByte(ref p, (byte)0, ref IoError);
                retVal = sendByte(ref p, (byte)0, ref IoError);

                ManufacturerID = sendByte(ref p, (byte)0x80, ref IoError);
                DeviceID = sendByte(ref p, (byte)0x80, ref IoError);
            }
            catch (Exception ex)
            {
                stat.AppendText("[ERROR]: " + ex.Message + "\r\n");
                stat.AppendText("[REQUEST FAILED] \r\n\r\n");
                p.Close();
                return;
            }

            stat.AppendText(String.Format("Manufacturer ID: {0:D0} (0x{0:X2})\r\n", ManufacturerID));
            stat.AppendText(String.Format("Device ID: {0:D0} (0x{0:X2})\r\n", DeviceID));

            stat.AppendText("\r\n\r\n");
            p.Close();
        }

        //-----------------------------------------------------------------------------------------
        /* ОЧИСТИТЬ ОКНО ЛОГГИРОВАНИЯ */
        private void btnClearLog_Click(object sender, EventArgs e)
        {
            stat.Text = "";
        }

        //-----------------------------------------------------------------------------------------
        /* СЧИТАТЬ ДАННЫЕ ИЗ ПАМЯТИ В ФАЙЛ */
        private void btnReadMemoryToFile_Click(object sender, EventArgs e)
        {
            SaveFileDialog sd = new SaveFileDialog();
            sd.DefaultExt = "bin";
            sd.Filter = "Binary files (*.bin) | *.bin";
            sd.InitialDirectory = Application.StartupPath;
            if (sd.ShowDialog() == System.Windows.Forms.DialogResult.Cancel) return;

            btnReadManufID.PerformClick();

            stat.AppendText("Reading ...\r\n");

            int AFrom = Int32.Parse(tbAR1.Text, System.Globalization.NumberStyles.HexNumber);
            int ATo = Int32.Parse(tbAR2.Text, System.Globalization.NumberStyles.HexNumber);

            SerialPort p = new SerialPort(cbPort.Text, Convert.ToInt32(tbSerSpeed.Text), Parity.None, 8, StopBits.One);
            p.Handshake = Handshake.None;
            p.RtsEnable = false;
            p.ReadTimeout = 1000;
            p.WriteTimeout = 1000;

            try
            {
                p.Open();
            }
            catch (Exception ex)
            {
                stat.AppendText("[ERROR]: " + ex.Message + "\r\n\r\n");
                return;
            }

            bool IoError = true;
            int retVal;

            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)4, ref IoError);
            retVal = sendByte(ref p, (byte)0xFF, ref IoError);
            retVal = sendByte(ref p, (byte)SPIMEM.MEM_READ_DATA, ref IoError);
            retVal = sendByte(ref p, (byte)(AFrom >> 16), ref IoError);
            retVal = sendByte(ref p, (byte)(AFrom >> 8), ref IoError);
            retVal = sendByte(ref p, (byte)(AFrom), ref IoError);

            using (BinaryWriter writer = new BinaryWriter(File.Open(sd.FileName, FileMode.Create)))
            {
                pb.Value = 0;
                pb.Minimum = 0;
                pb.Maximum = ATo - AFrom + 1;

                for (int i = AFrom; i <= ATo; i++)
                {
                    retVal = sendByte(ref p, (byte)0x80, ref IoError);
                    writer.Write((byte)retVal);
                    if (pb.Value < pb.Maximum) pb.Value++;
                    Application.DoEvents();

                }
                retVal = sendByte(ref p, (byte)0xFF, ref IoError);
            }

            stat.AppendText("[DONE]\r\n\r\n");
            p.Close();
        }

        //-----------------------------------------------------------------------------------------
        /* ОЧИСТИТЬ СЕКТОР 4096 БАЙТ */
        private void btnEraseSector_Click(object sender, EventArgs e)
        {
            btnReadManufID.PerformClick();
            stat.AppendText("Erasing sector 0x" + tbASector.Text + "...\r\n");

            int sectorAddr = Int32.Parse(tbASector.Text, System.Globalization.NumberStyles.HexNumber);
            sectorAddr = sectorAddr * 0x1000;

            SerialPort p = new SerialPort(cbPort.Text, Convert.ToInt32(tbSerSpeed.Text), Parity.None, 8, StopBits.One);
            p.Handshake = Handshake.None;
            p.RtsEnable = false;
            p.ReadTimeout = 1000;
            p.WriteTimeout = 1000;

            try
            {
                p.Open();
            }
            catch (Exception ex)
            {
                stat.AppendText("[ERROR]: " + ex.Message + "\r\n");
                return;
            }

            bool IoError = true;
            int retVal;

            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)1, ref IoError);
            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)SPIMEM.MEM_WRITE_ENABLE, ref IoError);

            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)1, ref IoError);
            retVal = sendByte(ref p, (byte)1, ref IoError);
            retVal = sendByte(ref p, (byte)SPIMEM.MEM_READ_STAT_REG1, ref IoError);
            retVal = sendByte(ref p, (byte)0x80, ref IoError);
            if ((retVal & 0x02) != 0x02)
            {
                stat.AppendText("[ERROR: ] Cannot put MEM into write status.\r\n\r\n");
                p.Close();
                return;
            }

            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)4, ref IoError);
            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)SPIMEM.MEM_SEC_ERASE_4K, ref IoError);
            retVal = sendByte(ref p, (byte)(sectorAddr >> 16), ref IoError);
            retVal = sendByte(ref p, (byte)(sectorAddr >> 8), ref IoError);
            retVal = sendByte(ref p, (byte)(sectorAddr), ref IoError);

            stat.AppendText("Busy...\r\n");

            do {
                retVal = sendByte(ref p, (byte)0, ref IoError);
                retVal = sendByte(ref p, (byte)1, ref IoError);
                retVal = sendByte(ref p, (byte)1, ref IoError);
                retVal = sendByte(ref p, (byte)SPIMEM.MEM_READ_STAT_REG1, ref IoError);
                retVal = sendByte(ref p, (byte)0x80, ref IoError);
            } while ((retVal & 0x01) != 0x01);

            stat.AppendText("[DONE]\r\n\r\n");
            p.Close();
        }

        //-----------------------------------------------------------------------------------------
        /* ОЧИСТИТЬ ВЕСЬ ЧИП */
        private void btnEraseChip_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Все данные будут уничтожены. Продолжить?", "Стирание памяти", MessageBoxButtons.YesNo) ==
                System.Windows.Forms.DialogResult.No) return;

            btnReadManufID.PerformClick();
            stat.AppendText("Erasing entire chip...\r\n");

            SerialPort p = new SerialPort(cbPort.Text, Convert.ToInt32(tbSerSpeed.Text), Parity.None, 8, StopBits.One);
            p.Handshake = Handshake.None;
            p.RtsEnable = false;
            p.ReadTimeout = 1000;
            p.WriteTimeout = 1000;

            try
            {
                p.Open();
            }
            catch (Exception ex)
            {
                stat.AppendText("[ERROR]: " + ex.Message + "\r\n");
                return;
            }

            bool IoError = true;
            int retVal;

            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)1, ref IoError);
            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)SPIMEM.MEM_WRITE_ENABLE, ref IoError);

            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)1, ref IoError);
            retVal = sendByte(ref p, (byte)1, ref IoError);
            retVal = sendByte(ref p, (byte)SPIMEM.MEM_READ_STAT_REG1, ref IoError);
            retVal = sendByte(ref p, (byte)0x80, ref IoError);
            if ((retVal & 0x02) != 0x02)
            {
                stat.AppendText("[ERROR: ] Cannot put MEM into write status.\r\n\r\n");
                p.Close();
                return;
            }

            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)1, ref IoError);
            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)SPIMEM.MEM_CHIP_ERASE_1, ref IoError);

            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)1, ref IoError);
            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)SPIMEM.MEM_CHIP_ERASE_2, ref IoError);

            do
            {
                retVal = sendByte(ref p, (byte)0, ref IoError);
                retVal = sendByte(ref p, (byte)1, ref IoError);
                retVal = sendByte(ref p, (byte)1, ref IoError);
                retVal = sendByte(ref p, (byte)SPIMEM.MEM_READ_STAT_REG1, ref IoError);
                retVal = sendByte(ref p, (byte)0x80, ref IoError);
                Application.DoEvents();
            } while ((retVal & 0x01) == 0x01);

            stat.AppendText("[DONE]\r\n\r\n");
            p.Close();
        }

        //-----------------------------------------------------------------------------------------
        /* ЗАПИСАТЬ ДАННЫЕ В ПАМЯТЬ */
        private void btnWriteFile_Click(object sender, EventArgs e)
        {
            OpenFileDialog od = new OpenFileDialog();
            od.DefaultExt = "bin";
            od.Filter = "Binary files (*.bin) | *.bin";
            od.InitialDirectory = Application.StartupPath;
            if (od.ShowDialog() == System.Windows.Forms.DialogResult.Cancel) return;

            btnReadManufID.PerformClick();

            stat.AppendText("Writing ...\r\n");

            int AFrom = Int32.Parse(tbWriteFromAddr.Text, System.Globalization.NumberStyles.HexNumber);
            int ACount = 0;

            SerialPort p = new SerialPort(cbPort.Text, Convert.ToInt32(tbSerSpeed.Text), Parity.None, 8, StopBits.One);
            p.Handshake = Handshake.None;
            p.RtsEnable = false;
            p.ReadTimeout = 1000;
            p.WriteTimeout = 1000;

            try
            {
                p.Open();
            }
            catch (Exception ex)
            {
                stat.AppendText("[ERROR]: " + ex.Message + "\r\n\r\n");
                return;
            }

            bool IoError = true;
            int retVal;

            using (BinaryReader reader = new BinaryReader(File.Open(od.FileName, FileMode.Open)))
            {
                int len = (int)reader.BaseStream.Length;
                stat.AppendText("File size: " + len.ToString() + " bytes\r\n");
                int pages = (int)Math.Ceiling((double)(len / 256.0));
                stat.AppendText("Pages to write: " + pages.ToString() + "\r\n");

                pb.Value = 0;
                pb.Minimum = 0;
                pb.Maximum = len;

                int currByteNr = 0;
                byte b = 0;

                stat.AppendText("Page 1 of " + pages.ToString());

                for (int page = 0; page < pages; page++)
                {

                    string[] lines = stat.Lines;
                    lines[stat.Lines.Length-1] = string.Format("Page {0:D0} of {1:D0}", page+1, pages);
                    stat.Lines = lines;

                    //переведем в режим Write Enable
                    retVal = sendByte(ref p, (byte)0, ref IoError);
                    retVal = sendByte(ref p, (byte)1, ref IoError);
                    retVal = sendByte(ref p, (byte)0, ref IoError);
                    retVal = sendByte(ref p, (byte)SPIMEM.MEM_WRITE_ENABLE, ref IoError);

                    //проверим состояние Write Enable
                    retVal = sendByte(ref p, (byte)0, ref IoError);
                    retVal = sendByte(ref p, (byte)1, ref IoError);
                    retVal = sendByte(ref p, (byte)1, ref IoError);
                    retVal = sendByte(ref p, (byte)SPIMEM.MEM_READ_STAT_REG1, ref IoError);
                    retVal = sendByte(ref p, (byte)0x80, ref IoError);
                    if ((retVal & 0x02) != 0x02)
                    {
                        stat.AppendText("[ERROR: ] Cannot put MEM into write status.\r\n\r\n");
                        p.Close();
                        return;
                    }

                    //команда записи 256 байт
                    int pageAddr = page * 256;
                    retVal = sendByte(ref p, (byte)0x01, ref IoError); //cmd len = 0x0104
                    retVal = sendByte(ref p, (byte)0x04, ref IoError);
                    retVal = sendByte(ref p, (byte)0, ref IoError);
                    retVal = sendByte(ref p, (byte)SPIMEM.MEM_PAGE_PROGRAM, ref IoError);
                    retVal = sendByte(ref p, (byte)(pageAddr >> 16), ref IoError);
                    retVal = sendByte(ref p, (byte)(pageAddr >> 8), ref IoError);
                    retVal = sendByte(ref p, (byte)(pageAddr), ref IoError);

                    for (int i = 0; i < 256; i++)
                    {
                        //байт из файла или 0xFF, что бы дописать страницу
                        if (currByteNr++ < len)
                        {
                            b = reader.ReadByte();
                        }
                        else
                        {
                            b = 0xFF;
                        }

                        retVal = sendByte(ref p, (byte)b, ref IoError);
                        if (pb.Value < pb.Maximum) pb.Value++;
                        Application.DoEvents();
                    }

                    //ждем пока данные запрограммируются
                    do
                    {
                        retVal = sendByte(ref p, (byte)0, ref IoError);
                        retVal = sendByte(ref p, (byte)1, ref IoError);
                        retVal = sendByte(ref p, (byte)1, ref IoError);
                        retVal = sendByte(ref p, (byte)SPIMEM.MEM_READ_STAT_REG1, ref IoError);
                        retVal = sendByte(ref p, (byte)0x80, ref IoError);
                        Application.DoEvents();
                    } while ((retVal & 0x01) == 0x01);
                }
            }

            stat.AppendText("\r\n[DONE]\r\n\r\n");
            p.Close();
        }

        //-----------------------------------------------------------------------------------------
        /* ОТКЛЮЧИТЬ СОСТОЯНИЕ ЗАПИСИ */
        private void btnDisableWrite_Click(object sender, EventArgs e)
        {
            SerialPort p = new SerialPort(cbPort.Text, Convert.ToInt32(tbSerSpeed.Text), Parity.None, 8, StopBits.One);
            p.Handshake = Handshake.None;
            p.RtsEnable = false;
            p.ReadTimeout = 1000;
            p.WriteTimeout = 1000;

            try
            {
                p.Open();
            }
            catch (Exception ex)
            {
                stat.AppendText("[ERROR]: " + ex.Message + "\r\n");
                return;
            }

            bool IoError = true;
            int retVal;

            stat.AppendText("Try to disable write state...\r\n");

            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)1, ref IoError);
            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)SPIMEM.MEM_WRITE_DISABLE, ref IoError);

            retVal = sendByte(ref p, (byte)0, ref IoError);
            retVal = sendByte(ref p, (byte)1, ref IoError);
            retVal = sendByte(ref p, (byte)1, ref IoError);
            retVal = sendByte(ref p, (byte)SPIMEM.MEM_READ_STAT_REG1, ref IoError);
            retVal = sendByte(ref p, (byte)0x80, ref IoError);
            if ((retVal & 0x02) == 0x02)
            {
                stat.AppendText("[ERROR: ] Cannot disable write status.\r\n\r\n");
                p.Close();
                return;
            }

            stat.AppendText("[DONE]\r\n\r\n");
            p.Close();
        }

        
    }
}
