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
using System.IO;


namespace flasher
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
        };

        SerialPort p;
        byte[] buf = new byte[256 * 16]; //4k sector
        TMem mem = new TMem(); //эмулятор памяти

        public Form1()
        {
            InitializeComponent();

            //get all available com ports
            foreach (string s in System.IO.Ports.SerialPort.GetPortNames())
            {
                cbComPort.Items.Add(s);
            }

            if (cbComPort.Items.Count > 0) cbComPort.SelectedIndex = 0;
        }

        //-----------------------------------------------------------------------------------------
        /* подключение к COM порту */
        private void btnComConnect_Click(object sender, EventArgs e)
        {
            if (!cbEmulate.Checked)
            {
                if (cbComPort.Text.Trim() == "") return;

                Msg("Connecting to " + cbComPort.Text + "...", false);
                p = new SerialPort(cbComPort.Text, Convert.ToInt32(tbComSpeed.Text), Parity.None, 8, StopBits.One);
                p.Handshake = Handshake.None;
                p.RtsEnable = false;
                p.ReadTimeout = 2000; //2s
                p.WriteTimeout = 1000; //1s

                try
                {
                    p.Open();
                }
                catch (Exception ex)
                {
                    Msg(" failed.", true);
                    Msg("[ERROR] " + ex.Message, true);
                    return;
                }
            }
            else
            {
                Msg("Enable emulating ROM...", false);

                try
                {
                    mem.Open();
                }
                catch (Exception ex)
                {
                    Msg(" failed.", true);
                    Msg("[ERROR] " + ex.Message, true);
                    return;
                }
            }

            cbEmulate.Enabled = false;
            btnComDisconnect.Enabled = true;
            btnComConnect.Enabled = false;

            SpiSend(11, 0); //CS=1
            SpiSend(21, 0); //HOLD=1

            Msg(" done.", true);
        }

        //-----------------------------------------------------------------------------------------
        private void btnComDisconnect_Click(object sender, EventArgs e)
        {
            if (!cbEmulate.Checked)
            {
                SpiSend(11, 0); //CS=1

                p.Close();
            }
            else
            {
                mem.Close();
            }

            cbEmulate.Enabled = true;
            btnComDisconnect.Enabled = false;
            btnComConnect.Enabled = true;
        
            Msg("Disconnected.", true);
        }

        //-----------------------------------------------------------------------------------------
        void Msg(string message, bool newLine)
        {
            tbMessages.AppendText(message);
            if (newLine) tbMessages.AppendText("\r\n");
            Application.DoEvents();
        }

        //-----------------------------------------------------------------------------------------
        /*отправляет команду в MCU. возвращает -1 в случае ошибки или считанное значение из SPI*/
        int SpiSend(byte prefix, byte data)
        {
            int retval = data;

            //обработка режима эмуляции или обращения к памяти
            if (cbEmulate.Checked)
            {
                retval = mem.SpiSend(prefix, data);
            }
            else
            {
                //оправляем префикс
                retval = WriteSingleByte(prefix);
                if (retval > 255) return -1; //ошибка записи

                if (prefix == 0xAA) //отправляем байт данных в SPI
                {
                    retval = WriteSingleByte(data);
                    if (retval > 255) return -1; //ошибка записи               
                }
            }

            return retval;
        }

        //-----------------------------------------------------------------------------------------
        /* отправляет байт в COM порт, после этого считывает 2 байта из порта
         возвращает 256 если операция чтения\записи завершилась с ошибкой.
         возвращает 257 если первое возвращенное число не равно отправленному
         возвращает второе число <256 если все прошло удачно
         */
        int WriteSingleByte(byte b)
        {
            byte retval;

            try
            {
                //отправили байт
                p.Write(new byte[] { b }, 0, 1);
            }
            catch
            {
                //выход по таймауту или еще каким причинам
                return 256;
            }

            Application.DoEvents();

            try
            {
                retval = (byte)p.ReadByte();
            }
            catch
            {
                //выход по таймауту или еще каким причинам
                return 256;
            }

            if (retval != b) return 257;

            Application.DoEvents();

            try
            {
                retval = (byte)p.ReadByte();
            }
            catch
            {
                //выход по таймауту или еще каким причинам
                return 256;
            }

            Application.DoEvents();
            return retval;
        }

        //-----------------------------------------------------------------------------------------
        int WriteSingleByteManually(byte b)
        {
            int result = 0;
            byte retval;

            try
            {
                p.Write(new byte[] { b }, 0, 1);
            }
            catch
            {
                return 256;
            }

            Application.DoEvents();

            try
            {
                retval = (byte)p.ReadByte();
            }
            catch
            {
                return 256;
            }

            result = retval << 8;

            Application.DoEvents();

            try
            {
                retval = (byte)p.ReadByte();
            }
            catch
            {
                return 256;
            }

            Application.DoEvents();

            result |= retval;
            return result;
        }

        //-----------------------------------------------------------------------------------------
        /*оправить байт вручную в MCU*/
        private void btnSendByte_Click(object sender, EventArgs e)
        {
            if (cbEmulate.Checked)
            {
                Msg("[INFO] Does not work in emulation mode", true);
                return;
            }

            if (tbByteToSend.Text.Trim() == "") return;

            lblStatus.Text = "WAIT...";
            Application.DoEvents();

            try { p.BaseStream.Flush(); } catch { };

            int retval = WriteSingleByteManually(Convert.ToByte(tbByteToSend.Text.Trim()));
            if (retval == 256)
                lblStatus.Text = "ERROR";
            else
            {
                lblStatus.Text = "DONE";
                tbByteToRead1.Text = (retval >> 8).ToString();
                tbByteToRead2.Text = (retval & 0xFF).ToString();
            }
        }

        //-----------------------------------------------------------------------------------------
        /*установить сигнал CS=0 вручную */
        private void btnCS1_Click(object sender, EventArgs e)
        {
            lblStatus.Text = "WAIT...";
            Application.DoEvents();

            try { p.BaseStream.Flush(); } catch { };

            int retval = SpiSend(10, 0);
            if (retval < 0)
                lblStatus.Text = "ERROR";
            else
                lblStatus.Text = "DONE";
        }

        //-----------------------------------------------------------------------------------------
        /*установить сигнал CS=1*/
        private void btnCS0_Click(object sender, EventArgs e)
        {
            lblStatus.Text = "WAIT...";
            Application.DoEvents();

            try { p.BaseStream.Flush(); } catch { };

            int retval = SpiSend(11, 0);
            if (retval < 0)
                lblStatus.Text = "ERROR";
            else
                lblStatus.Text = "DONE";
        }

        //-----------------------------------------------------------------------------------------
        /*считать данные о чипе*/
        private void btnGetChipInfo_Click(object sender, EventArgs e)
        {
            if (cbEmulate.Checked)
            {
                Msg("[INFO] Does not work in emulation mode", true);
                return;
            }

            int retval;
            int deviceID = 0;
            int manufacturerID = 0;
            Int64 chipID = 0;

            if (p == null) return;
            if (!p.IsOpen) return;
            try { p.BaseStream.Flush(); } catch { };
            Msg("[CMD] Get chip information...", false);

            try
            {
                //--- MANUFACTURER AND DEVICE ID ---
                //enable chip
                if (SpiSend(10, 0) < 0) throw new Exception();
                //90h
                if (SpiSend(0xAA, 0x90) < 0) throw new Exception();
                //dummy
                if (SpiSend(0xAA, 0x00) < 0) throw new Exception();
                if (SpiSend(0xAA, 0x00) < 0) throw new Exception();
                //0x00
                if (SpiSend(0xAA, 0x00) < 0) throw new Exception();
                //manufacturer id
                retval = SpiSend(0xAA, 0xFF);
                if (retval < 0) throw new Exception();
                manufacturerID = retval;
                //device id
                retval = SpiSend(0xAA, 0xFF);
                if (retval < 0) throw new Exception();
                deviceID = retval;
                //disable chip
                if (SpiSend(11, 0) < 0) throw new Exception();

                //--- CHIP ID ---
                //enable chip
                if (SpiSend(10, 0) < 0) throw new Exception();
                //90h
                if (SpiSend(0xAA, 0x4B) < 0) throw new Exception();
                //dummy
                if (SpiSend(0xAA, 0x00) < 0) throw new Exception();
                if (SpiSend(0xAA, 0x00) < 0) throw new Exception();
                if (SpiSend(0xAA, 0x00) < 0) throw new Exception();
                if (SpiSend(0xAA, 0x00) < 0) throw new Exception();
                //unique chip id
                for (int i=0; i<8; i++)
                {
                    retval = SpiSend(0xAA, 0xFF);
                    if (retval < 0) throw new Exception();
                    chipID <<= 8;
                    chipID |= (Int64)retval;
                }
                //disable chip
                if (SpiSend(11, 0) < 0) throw new Exception();

                Msg(" done.", true);
                Msg(string.Format("[INFO] Manufacturer ID=0x{0:X2}", manufacturerID), true);
                Msg(string.Format("[INFO] Device ID=0x{0:X2}", deviceID), true);
                Msg(string.Format("[INFO] Chip unique ID=0x{0:X16}", chipID), true);
            }
            catch
            {
                Msg(" failed.", true);
                SpiSend(11, 0);
            }
            

        }

        //-----------------------------------------------------------------------------------------
        /*очистить всю память*/
        private void btnEraseWholeMem_Click(object sender, EventArgs e)
        {
            if (!cbEmulate.Checked)
            {
                if (p == null) return;
                if (!p.IsOpen) return;
                try { p.BaseStream.Flush(); } catch { };
            }
            if (MessageBox.Show("Do you really want to erase the whole memory?", "Clear whole memory", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.No) return;

            Msg("[CMD] Erasing memory...", false);

            try
            {
                //--- ENABLE WRITE ---
                //enable chip
                if (SpiSend(10, 0) < 0) throw new Exception();
                //06h
                if (SpiSend(0xAA, 0x06) < 0) throw new Exception();
                //disable chip
                if (SpiSend(11, 0) < 0) throw new Exception();

                //--- CHIP ERASE ---
                //enable chip
                if (SpiSend(10, 0) < 0) throw new Exception();
                //C7h
                if (SpiSend(0xAA, 0xC7) < 0) throw new Exception();
                //disable chip
                if (SpiSend(11, 0) < 0) throw new Exception();

                //--- waiting for compeletion ---
                waitForBUSYoff();

                //--- DISABLE WRITE ---
                //enable chip
                if (SpiSend(10, 0) < 0) throw new Exception();
                //04h
                if (SpiSend(0xAA, 0x04) < 0) throw new Exception();
                //disable chip
                if (SpiSend(11, 0) < 0) throw new Exception();

                Msg(" done.", true);
            }
            catch
            {
                Msg(" failed.", true);
                SpiSend(11, 0);
            }
        }

        //-----------------------------------------------------------------------------------------
        /*очистить окно вывода сообщений*/
        private void tbMessages_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Right)
                tbMessages.Clear();
        }

        //-----------------------------------------------------------------------------------------
        /*ожидание бита статуса ГОТОВ*/
        private void waitForBUSYoff()
        {
            int regisry;

            try
            {
                do
                {
                    //--- READ STATUS REGISTER 1 ---
                    //enable chip
                    if (SpiSend(10, 0) < 0) throw new Exception();
                    //05h
                    if (SpiSend(0xAA, 0x05) < 0) throw new Exception();
                    //read register
                    regisry = SpiSend(0xAA, 0xFF);
                    if (regisry < 0) throw new Exception();
                    //disable chip
                    if (SpiSend(11, 0) < 0) throw new Exception();

                    Application.DoEvents();
                } while ((regisry & 0x01) == 0x01);
            }
            catch
            {
                Msg("[ERR] Read BUSY status bit is failed.", true);
                SpiSend(11, 0);
            }
        }

        //-----------------------------------------------------------------------------------------
        /*считывает память с указанного адреса и записывает в файл dump.dat и dump.txt*/
        private void btnRedMemToFile_Click(object sender, EventArgs e)
        {
            int retval;

            if (!cbEmulate.Checked)
            {
                if (p == null) return;
                if (!p.IsOpen) return;
                try { p.BaseStream.Flush(); } catch { };
            }
            Msg("[CMD] Reading to file 'dump.dat'... ", false);

            int address = Convert.ToInt32(tbAddressStart.Text);
            int length = Convert.ToInt32(tbAddressLength.Text);
            if (length == 0)
            {
                Msg("failed.", true);
                Msg("[ERR] Length cannot be zero.", true);
                return;
            }

            FileStream fs = new FileStream(Application.StartupPath + @"\dump.dat", FileMode.Create);

            try
            {
                btnAbort.Enabled = true;
                pb.Maximum = length;
                btnAbort.Tag = (bool)false;

                //enable chip
                if (SpiSend(10, 0) < 0) throw new Exception();
                //03h
                if (SpiSend(0xAA, 0x0B) < 0) throw new Exception();
                //send address
                if (SpiSend(0xAA, (byte)(address >> 16)) < 0) throw new Exception();
                if (SpiSend(0xAA, (byte)(address >> 8)) < 0) throw new Exception();
                if (SpiSend(0xAA, (byte)(address)) < 0) throw new Exception();
                //dummy
                if (SpiSend(0xAA, 0x00) < 0) throw new Exception();
                //read byte(s)
                do
                {
                    retval = SpiSend(0xAA, 0xFF);
                    if (retval < 0) throw new Exception();
                    fs.WriteByte((byte)retval);
                    length--;
                    pb.Value++;
                    Application.DoEvents();
                    if ((bool)(btnAbort.Tag) == true) break;
                } while (length > 0);
                //disable chip
                if (SpiSend(11, 0) < 0) throw new Exception();

                if ((bool)(btnAbort.Tag) == true)
                    Msg("aborted.", true);
                else
                    Msg("done.", true);
                pb.Value = 0;
            }
            catch
            {
                Msg(" failed.", true);
                SpiSend(11, 0);
                pb.Value = 0;
                btnAbort.Enabled = false;
            }

            fs.Close();
        }

        //-----------------------------------------------------------------------------------------
        /*считывает байт с указанного адреса*/
        private void btnReadByte_Click(object sender, EventArgs e)
        {
            int retval;

            if (!cbEmulate.Checked)
            {
                if (p == null) return;
                if (!p.IsOpen) return;
                try { p.BaseStream.Flush(); } catch { };
            }
            lblStatus.Text = "WAIT...";

            try
            {
                //detect address
                int address = Convert.ToInt32(tbAddressStart.Text);
                //enable chip
                if (SpiSend(10, 0) < 0) throw new Exception();
                //03h
                if (SpiSend(0xAA, 0x03) < 0) throw new Exception();
                //send address
                if (SpiSend(0xAA, (byte)(address >> 16)) < 0) throw new Exception();
                if (SpiSend(0xAA, (byte)(address >> 8)) < 0) throw new Exception();
                if (SpiSend(0xAA, (byte)(address)) < 0) throw new Exception();
                //read byte
                retval = SpiSend(0xAA, 0xFF);
                if (retval < 0) throw new Exception();
                tbByteToRW.Text = retval.ToString();
                //disable chip
                if (SpiSend(11, 0) < 0) throw new Exception();

                lblStatus.Text = "DONE";
            }
            catch
            {
                lblStatus.Text = "FAILED";
                SpiSend(11, 0);
            }
        }

        //-----------------------------------------------------------------------------------------
        /*записывает байт в указанный адрес*/
        private void btnWriteByte_Click(object sender, EventArgs e)
        {
            int retval;

            if (!cbEmulate.Checked)
            {
                if (p == null) return;
                if (!p.IsOpen) return;
                try { p.BaseStream.Flush(); } catch { };
            }
            lblStatus.Text = "WAIT...";

            try
            {
                //detect address
                int address = Convert.ToInt32(tbAddressStart.Text);

                //--- ENABLE WRITE ---
                //enable chip
                if (SpiSend(10, 0) < 0) throw new Exception();
                //06h
                if (SpiSend(0xAA, 0x06) < 0) throw new Exception();
                //disable chip
                if (SpiSend(11, 0) < 0) throw new Exception();

                //--- WRITE BYTE ---
                //enable chip
                if (SpiSend(10, 0) < 0) throw new Exception();
                //02h
                if (SpiSend(0xAA, 0x02) < 0) throw new Exception();
                //send address
                if (SpiSend(0xAA, (byte)(address >> 16)) < 0) throw new Exception();
                if (SpiSend(0xAA, (byte)(address >> 8)) < 0) throw new Exception();
                if (SpiSend(0xAA, (byte)(address)) < 0) throw new Exception();
                //write byte
                if (SpiSend(0xAA, Convert.ToByte(tbByteToRW.Text)) < 0) throw new Exception();
                //disable chip
                if (SpiSend(11, 0) < 0) throw new Exception();

                //--- waiting for compeletion ---
                waitForBUSYoff();

                //--- DISABLE WRITE ---
                //enable chip
                if (SpiSend(10, 0) < 0) throw new Exception();
                //04h
                if (SpiSend(0xAA, 0x04) < 0) throw new Exception();
                //disable chip
                if (SpiSend(11, 0) < 0) throw new Exception();

                lblStatus.Text = "DONE";
            }
            catch
            {
                lblStatus.Text = "FAILED";
                SpiSend(11, 0);
            }
        }

        //-----------------------------------------------------------------------------------------
        /*прервать текущую операцию*/
        private void btnAbort_Click(object sender, EventArgs e)
        {
            btnAbort.Tag = (bool)true;
        }

        //-----------------------------------------------------------------------------------------
        /*очистить часть памяти */
        private void btnClearMem_Click(object sender, EventArgs e)
        {
            int retval;

            if (!cbEmulate.Checked)
            {
                if (p == null) return;
                if (!p.IsOpen) return;
                try { p.BaseStream.Flush(); } catch { };
            }
            if (MessageBox.Show("Do you really want to erase the selected memory range?", "Clear memory", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.No) return;

            Msg("[CMD] Clearing memory... ", false);

            int address = Convert.ToInt32(tbAddressStart.Text);
            int length = Convert.ToInt32(tbAddressLength.Text);
            if (length == 0)
            {
                Msg("failed.", true);
                Msg("[ERR] Length cannot be zero.", true);
                return;
            }

            //определим первый сектор для очистки
            int firstSectorToClear = address / (256*16);
            int lastSectorToClear = (address + length - 1) / (256 * 16);
            int sectorsToClear = lastSectorToClear - firstSectorToClear + 1;
            int pages = sectorsToClear * 16;
            int pageFirst = (address / 256) - (firstSectorToClear * 16);
            int pageLast = (address + length) / 256 - (firstSectorToClear * 16);
            int firstPageAddr = 0;
            int firstPageLen = address - (pageFirst * 256 + firstSectorToClear * 4096);
            int lastPageAddr = (address + length) - (pageLast * 256 + firstSectorToClear * 4096);
            int lastPageLen = 256 - lastPageAddr;


            int sectorAddr;
            int pageAddr;

            pb.Maximum = 2 * (pages * 256); //store, restore
            btnAbort.Enabled = true;
            btnAbort.Tag = (bool)false;

            try
            {
                for (int sector=0; sector<sectorsToClear; sector++)
                {
                    sectorAddr = sector * (256 * 16);

                    //--- SAVE SECTOR DATA ---
                    if (SpiSend(10, 0) < 0) throw new Exception();
                    if (SpiSend(0xAA, 0x0B) < 0) throw new Exception();
                    if (SpiSend(0xAA, (byte)(sectorAddr >> 16)) < 0) throw new Exception();
                    if (SpiSend(0xAA, (byte)(sectorAddr >> 8)) < 0) throw new Exception();
                    if (SpiSend(0xAA, (byte)(sectorAddr)) < 0) throw new Exception();
                    if (SpiSend(0xAA, 0x00) < 0) throw new Exception();
                    for (int a=0; a<(256*16); a++)
                    {
                        retval = SpiSend(0xAA, 0xFF);
                        if (retval < 0) throw new Exception();
                        buf[a] = (byte)retval;
                        pb.Value++;
                        Application.DoEvents();
                        if ((bool)(btnAbort.Tag) == true) throw new Exception("abort");
                    }
                    if (SpiSend(11, 0) < 0) throw new Exception();

                    //--- ENABLE WRITE ---
                    //enable chip
                    if (SpiSend(10, 0) < 0) throw new Exception();
                    //06h
                    if (SpiSend(0xAA, 0x06) < 0) throw new Exception();
                    //disable chip
                    if (SpiSend(11, 0) < 0) throw new Exception();

                    //--- SECTOR ERASE ---
                    //enable chip
                    if (SpiSend(10, 0) < 0) throw new Exception();
                    //20h
                    if (SpiSend(0xAA, 0x20) < 0) throw new Exception();
                    if (SpiSend(0xAA, (byte)(sectorAddr >> 16)) < 0) throw new Exception();
                    if (SpiSend(0xAA, (byte)(sectorAddr >> 8)) < 0) throw new Exception();
                    if (SpiSend(0xAA, (byte)(sectorAddr)) < 0) throw new Exception();
                    //disable chip
                    if (SpiSend(11, 0) < 0) throw new Exception();

                    //--- waiting for compeletion ---
                    waitForBUSYoff();

                    //--- RESTORE DATA WITHOUT CLEARED DATA ---
                    for (int page = (sector * 16); page < (16*(sector + 1)); page++)
                    {
                        //--- ENABLE WRITE ---
                        //enable chip
                        if (SpiSend(10, 0) < 0) throw new Exception();
                        //06h
                        if (SpiSend(0xAA, 0x06) < 0) throw new Exception();
                        //disable chip
                        if (SpiSend(11, 0) < 0) throw new Exception();

                        //запись полных страниц
                        if ((page < pageFirst) || (page > pageLast))
                        {
                            //--- WRITE PAGE ---
                            pageAddr = page * 256;
                            if (SpiSend(10, 0) < 0) throw new Exception();
                            if (SpiSend(0xAA, 0x02) < 0) throw new Exception();
                            if (SpiSend(0xAA, (byte)(pageAddr >> 16)) < 0) throw new Exception();
                            if (SpiSend(0xAA, (byte)(pageAddr >> 8)) < 0) throw new Exception();
                            if (SpiSend(0xAA, (byte)(pageAddr)) < 0) throw new Exception();
                            for (int a = 0; a < 256; a++)
                            {
                                if (SpiSend(0xAA, buf[pageAddr + a]) < 0) throw new Exception();
                                pb.Value++;
                                Application.DoEvents();
                                if ((bool)(btnAbort.Tag) == true) throw new Exception("abort");

                            }
                            if (SpiSend(11, 0) < 0) throw new Exception();
                        }
                        else
                        {
                            //запись неполной первой страницы
                            if (page == pageFirst)
                            {
                                if (firstPageLen > 0)
                                {
                                    //--- WRITE PAGE ---
                                    pageAddr = page * 256 + firstPageAddr;
                                    if (SpiSend(10, 0) < 0) throw new Exception();
                                    if (SpiSend(0xAA, 0x02) < 0) throw new Exception();
                                    if (SpiSend(0xAA, (byte)(pageAddr >> 16)) < 0) throw new Exception();
                                    if (SpiSend(0xAA, (byte)(pageAddr >> 8)) < 0) throw new Exception();
                                    if (SpiSend(0xAA, (byte)(pageAddr)) < 0) throw new Exception();
                                    for (int a = 0; a < firstPageLen; a++)
                                    {
                                        if (SpiSend(0xAA, buf[pageAddr + a]) < 0) throw new Exception();
                                        pb.Value++;
                                        Application.DoEvents();
                                        if ((bool)(btnAbort.Tag) == true) throw new Exception("abort");

                                    }
                                    if (SpiSend(11, 0) < 0) throw new Exception();
                                }
                            }
                            //запись неполной последней страницы
                            if (page == pageLast)
                            {
                                if (lastPageLen > 0)
                                {
                                    //--- WRITE PAGE ---
                                    pageAddr = page * 256 + lastPageAddr;
                                    if (SpiSend(10, 0) < 0) throw new Exception();
                                    if (SpiSend(0xAA, 0x02) < 0) throw new Exception();
                                    if (SpiSend(0xAA, (byte)(pageAddr >> 16)) < 0) throw new Exception();
                                    if (SpiSend(0xAA, (byte)(pageAddr >> 8)) < 0) throw new Exception();
                                    if (SpiSend(0xAA, (byte)(pageAddr)) < 0) throw new Exception();
                                    for (int a = 0; a < lastPageLen; a++)
                                    {
                                        if (SpiSend(0xAA, buf[pageAddr + a]) < 0) throw new Exception();
                                        pb.Value++;
                                        Application.DoEvents();
                                        if ((bool)(btnAbort.Tag) == true) throw new Exception("abort");

                                    }
                                    if (SpiSend(11, 0) < 0) throw new Exception();
                                }
                            }
                        }

                        //--- waiting for compeletion ---
                        waitForBUSYoff();

                    }
                }

                //--- DISABLE WRITE ---
                //enable chip
                if (SpiSend(10, 0) < 0) throw new Exception();
                //04h
                if (SpiSend(0xAA, 0x04) < 0) throw new Exception();
                //disable chip
                if (SpiSend(11, 0) < 0) throw new Exception();

                Msg(" done.", true);
            }
            catch (Exception ex)
            {
                if (ex.Message == "abort")
                    Msg(" aborted.", true);
                else
                    Msg(" failed.", true);
                SpiSend(11, 0);
            }

            pb.Value = 0;
            btnAbort.Enabled = false;
        }

        //-----------------------------------------------------------------------------------------
        /*запись в память из указанного файла*/
        private void btnWriteMemFromFile_Click(object sender, EventArgs e)
        {
            if (!cbEmulate.Checked)
            {
                if (p == null) return;
                if (!p.IsOpen) return;
                try { p.BaseStream.Flush(); } catch { };
            }

            MessageBox.Show("You are about to write a data from a file to the memory.\r\n"+
                            "Please be sure that the memory area you want to write \r\nwas cleared "+
                            "(set to 0xFF) first. \r\nPlease select a file with a data.",
                            "Write to memory from file.", MessageBoxButtons.OK, MessageBoxIcon.Information);
            OpenFileDialog of = new OpenFileDialog();
            of.Multiselect = false;
            of.InitialDirectory = Application.StartupPath;
            if (of.ShowDialog() == DialogResult.Cancel) return;

            FileStream fs = new FileStream(of.FileName, FileMode.Open);
            fs.Position = 0;

            try
            {
                Msg("[CMD] Writing memory... ", false);

                int address = Convert.ToInt32(tbAddressStart.Text);
                int length = Convert.ToInt32(tbAddressLength.Text);
                int pageAddr;

                if (fs.Length > length)
                {
                    if (MessageBox.Show("The file size is bigger than the lenght specified. Number of bytes to be written" +
                                        " will be limited with the specified length. Continue?", "Warning",
                                        MessageBoxButtons.YesNo, MessageBoxIcon.Warning) == DialogResult.No)
                    {
                        throw new Exception("abort");
                    }
                    
                }

                if (fs.Length < length)
                {
                    if (MessageBox.Show("The file size is less than the lenght specified. Number of bytes to be written" +
                                        " will be set as the file size. Continue?", "Warning",
                                        MessageBoxButtons.YesNo, MessageBoxIcon.Warning) == DialogResult.No)
                    {
                        throw new Exception("abort");
                    }
                    length = (int)fs.Length;
                    tbAddressLength.Text = length.ToString();
                }

                pb.Maximum = length;
                btnAbort.Enabled = true;
                btnAbort.Tag = (bool)false;

                int firstSectorToWrite = address / (256 * 16);
                int pageFirst = (address / 256) - (firstSectorToWrite * 16);
                int pageLast = (address + length) / 256 - (firstSectorToWrite * 16);
                int pages = pageLast - pageFirst + 1;
                int firstPageAddr = address - (pageFirst * 256);
                int firstPageLen = ((firstPageAddr + length) > 255 ? (256 - firstPageAddr) : (length));
                int lastPageAddr = 0;
                int lastPageLen = (pages == 1 ? 0 : ((address + length) - (pageLast * 256 + firstSectorToWrite * 4096)));

                for (int page = pageFirst; page <= pageLast; page++)
                {
                    //--- ENABLE WRITE ---
                    //enable chip
                    if (SpiSend(10, 0) < 0) throw new Exception();
                    //06h
                    if (SpiSend(0xAA, 0x06) < 0) throw new Exception();
                    //disable chip
                    if (SpiSend(11, 0) < 0) throw new Exception();

                    //запись полных страниц
                    if ((page > pageFirst) && (page < pageLast))
                    {
                        //--- WRITE PAGE ---
                        pageAddr = page * 256;
                        if (SpiSend(10, 0) < 0) throw new Exception();
                        if (SpiSend(0xAA, 0x02) < 0) throw new Exception();
                        if (SpiSend(0xAA, (byte)(pageAddr >> 16)) < 0) throw new Exception();
                        if (SpiSend(0xAA, (byte)(pageAddr >> 8)) < 0) throw new Exception();
                        if (SpiSend(0xAA, (byte)(pageAddr)) < 0) throw new Exception();
                        for (int a = 0; a < 256; a++)
                        {
                            if (SpiSend(0xAA, (byte)fs.ReadByte()) < 0) throw new Exception();
                            pb.Value++;
                            Application.DoEvents();
                            if ((bool)(btnAbort.Tag) == true) throw new Exception("abort");

                        }
                        if (SpiSend(11, 0) < 0) throw new Exception();
                    }
                    else
                    {
                        //запись неполной первой страницы
                        if (page == pageFirst)
                        {
                            if (firstPageLen > 0)
                            {
                                //--- WRITE PAGE ---
                                pageAddr = page * 256 + firstPageAddr;
                                if (SpiSend(10, 0) < 0) throw new Exception();
                                if (SpiSend(0xAA, 0x02) < 0) throw new Exception();
                                if (SpiSend(0xAA, (byte)(pageAddr >> 16)) < 0) throw new Exception();
                                if (SpiSend(0xAA, (byte)(pageAddr >> 8)) < 0) throw new Exception();
                                if (SpiSend(0xAA, (byte)(pageAddr)) < 0) throw new Exception();
                                for (int a = 0; a < firstPageLen; a++)
                                {
                                    if (SpiSend(0xAA, (byte)fs.ReadByte()) < 0) throw new Exception();
                                    pb.Value++;
                                    Application.DoEvents();
                                    if ((bool)(btnAbort.Tag) == true) throw new Exception("abort");

                                }
                                if (SpiSend(11, 0) < 0) throw new Exception();
                            }
                        }
                        //запись неполной последней страницы
                        if (page == pageLast)
                        {
                            if (lastPageLen > 0)
                            {
                                //--- WRITE PAGE ---
                                pageAddr = page * 256 + lastPageAddr;
                                if (SpiSend(10, 0) < 0) throw new Exception();
                                if (SpiSend(0xAA, 0x02) < 0) throw new Exception();
                                if (SpiSend(0xAA, (byte)(pageAddr >> 16)) < 0) throw new Exception();
                                if (SpiSend(0xAA, (byte)(pageAddr >> 8)) < 0) throw new Exception();
                                if (SpiSend(0xAA, (byte)(pageAddr)) < 0) throw new Exception();
                                for (int a = 0; a < lastPageLen; a++)
                                {
                                    if (SpiSend(0xAA, (byte)fs.ReadByte()) < 0) throw new Exception();
                                    pb.Value++;
                                    Application.DoEvents();
                                    if ((bool)(btnAbort.Tag) == true) throw new Exception("abort");

                                }
                                if (SpiSend(11, 0) < 0) throw new Exception();
                            }
                        }
                    }

                    //--- waiting for compeletion ---
                    waitForBUSYoff();
                }

                Msg(" done.", true);
            }
            catch (Exception ex)
            {
                if (ex.Message == "abort")
                    Msg(" aborted.", true);
                else
                    Msg(" failed.", true);
                SpiSend(11, 0);
            }

            pb.Value = 0;
            btnAbort.Enabled = false;

            fs.Close();

        }

        //-----------------------------------------------------------------------------------------
        /*заполнить страницы значением */
        private void btnFillPages_Click(object sender, EventArgs e)
        {
            if (!cbEmulate.Checked)
            {
                if (p == null) return;
                if (!p.IsOpen) return;
                try { p.BaseStream.Flush(); } catch { };
            }
            if (MessageBox.Show("Please clear memory first. \r\nPlease enter a number of a first page in the address field. \r\nPlease put the required count of pages to be filled in the length field. \r\nProceed with filling?", "Fill pages", MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.No) return;

            int firstPage = Convert.ToInt32(tbAddressStart.Text);
            int pagesCount = Convert.ToInt32(tbAddressLength.Text);
            byte pattern = Convert.ToByte(tbFillPages.Text);

            if (cbFillPagesAuto.Checked)
                Msg("[CMD] Fill pages with auto pattern from 0 to 255... ", false);
            else
                Msg("[CMD] Fill pages with pattern '" + tbFillPages.Text + "'... ", false);

            pb.Maximum = 2 * (pagesCount * 256); //store, restore
            btnAbort.Enabled = true;
            btnAbort.Tag = (bool)false;

            try
            {
                for (int p = firstPage; p < pagesCount; p++)
                {
                    //--- ENABLE WRITE ---
                    //enable chip
                    if (SpiSend(10, 0) < 0) throw new Exception();
                    //06h
                    if (SpiSend(0xAA, 0x06) < 0) throw new Exception();
                    //disable chip
                    if (SpiSend(11, 0) < 0) throw new Exception();

                    //--- WRITE PAGE ---
                    int pageAddr = p * 256;
                    if (SpiSend(10, 0) < 0) throw new Exception();
                    if (SpiSend(0xAA, 0x02) < 0) throw new Exception();
                    if (SpiSend(0xAA, (byte)(pageAddr >> 16)) < 0) throw new Exception();
                    if (SpiSend(0xAA, (byte)(pageAddr >> 8)) < 0) throw new Exception();
                    if (SpiSend(0xAA, (byte)(pageAddr)) < 0) throw new Exception();
                    for (int a = 0; a < 256; a++)
                    {
                        if (SpiSend(0xAA, (cbFillPagesAuto.Checked ? (byte)(a) : pattern)) < 0) throw new Exception();

                        pb.Value++;
                        Application.DoEvents();
                        if ((bool)(btnAbort.Tag) == true) throw new Exception("abort");

                    }
                    if (SpiSend(11, 0) < 0) throw new Exception();

                    //--- waiting for compeletion ---
                    waitForBUSYoff();
                }

                Msg(" done.", true);
            }
            catch (Exception ex)
            {
                if (ex.Message == "abort")
                    Msg(" aborted.", true);
                else
                    Msg(" failed.", true);
                SpiSend(11, 0);
            }

            pb.Value = 0;
            btnAbort.Enabled = false;
        }

        //-----------------------------------------------------------------------------------------
        private void cbFillPagesAuto_CheckStateChanged(object sender, EventArgs e)
        {
            tbFillPages.Enabled = !cbFillPagesAuto.Checked;
        }

        //-----------------------------------------------------------------------------------------
        /* калькуляция размеров */
        private void tbAddressStart_TextChanged(object sender, EventArgs e)
        {
            int sector, sectors, pages;
            try
            {
                sector = Convert.ToInt32(tbAddressStart.Text) / (256 * 16);
                sectors = (Convert.ToInt32(tbAddressStart.Text) + Convert.ToInt32(tbAddressLength.Text)) / (256 * 16) - sector;
                pages = Convert.ToInt32(tbAddressLength.Text) / 256;

                lblStartSector.Text = "Start sector: " + sector.ToString();
                lblSectors.Text = "Sectors: " + sectors.ToString();
                lblPages.Text = "Pages: " + pages.ToString();
            }
            catch
            {
                lblStartSector.Text = "Start sector: <err>";
                lblSectors.Text = "Sectors: <err>";
                lblPages.Text = "Pages: <err>";
            }

        }

        //-----------------------------------------------------------------------------------------
        /* закрытие формы */
        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (p != null)
                if (p.IsOpen) p.Close();
            mem.Close();
        }
    }
}
