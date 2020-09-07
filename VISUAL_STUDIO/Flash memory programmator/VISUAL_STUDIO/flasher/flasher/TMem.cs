using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO;
using System.Windows.Forms;

namespace flasher
{
    class TMem
    {
        private FileStream fs;

        private bool writeEnable;
        private int CS;
        private byte[] buf;
        private int pdata;
        private int command;

        private int address;


        //---
        public void Open()
        {
            //проверка файла эмулятора памяти
            if (!File.Exists(Application.StartupPath + @"\mem.dat"))
            {
                fs = new FileStream(Application.StartupPath + @"\mem.dat", FileMode.Create);
                for (int i = 0; i < (32768 * 256); i++) fs.WriteByte(0xFF);
                fs.Position = 0;
            }
            else
            {
                fs = new FileStream(Application.StartupPath + @"\mem.dat", FileMode.Open);
            }
        }
        //---
        public void Close()
        {
            if (fs != null) fs.Close();
        }

        //---
        public TMem()
        {
            fs = null;
            writeEnable = false;
            CS = 1;
            buf = new byte[300];
            pdata = 0;
            command = 0;

        }

        //---
        public int SpiSend(byte prefix, byte data)
        {
            int retval = prefix;

            switch (prefix)
            {
                case 10:
                    if (CS == 1)
                    {
                        pdata = 0;
                        command = 0;
                        CS = 0;
                    }
                    break;

                case 11:
                    if (CS == 1) break;
                    switch (command)
                    {
                        case 0x06: //write enable
                            writeEnable = true;
                            break;

                        case 0x04: //write disable
                            writeEnable = false;
                            break;

                        case 0x20: //sector 4k erase
                            if (!writeEnable) break;
                            if (pdata < 3) break; //not enough data
                            address = 65536 * buf[0] + 256 * buf[1] + buf[2];
                            fs.Position = address;
                            for (int a = 0; a < 4096; a++)
                            {
                                fs.WriteByte(0xFF);
                            }
                            writeEnable = false;
                            fs.Flush();
                            break;

                        case 0x02: //page program
                            if (!writeEnable) break;
                            if (pdata < 3) break; //not enough data
                            address = 65536 * buf[0] + 256 * buf[1] + buf[2];
                            fs.Position = address;
                            for (int a=0; a<(pdata-3); a++)
                            {
                                fs.WriteByte(buf[3 + a]);
                            }
                            writeEnable = false;
                            fs.Flush();
                            break;

                        case 0xC7: //erase chip
                        case 0x60:
                            fs.Position = 0;
                            for (int i = 0; i < (32768 * 256); i++) fs.WriteByte(0xFF);
                            break;

                        default:
                            break;

                    }
                    pdata = 0;
                    command = 0;
                    CS = 1;
                    break;

                case 0xAA:
                    if (command == 0 && CS == 0)
                    {
                        command = data;
                    }
                    else
                    {
                        switch (command)
                        {
                            case 0x05: //read status register
                                if (pdata == 1) retval = (writeEnable ? 2 : 0);
                                break;

                            case 0x0B: //read data
                                if (pdata < 3)
                                {
                                    buf[pdata] = (byte)data;
                                    pdata++;
                                    break;
                                }
                                if (pdata == 3)
                                {
                                    address = 65536 * buf[0] + 256 * buf[1] + buf[2];
                                    fs.Position = address;
                                    pdata++;
                                }
                                else
                                {
                                    if (pdata == 4)
                                    {
                                        retval = fs.ReadByte();
                                    }
                                }
                                break;

                            case 0x03: //read data
                                if (pdata < 3)
                                {
                                    buf[pdata] = (byte)data;
                                    pdata++;
                                    break;
                                }
                                if (pdata == 3)
                                {
                                    address = 65536 * buf[0] + 256 * buf[1] + buf[2];
                                    fs.Position = address;
                                    retval = fs.ReadByte();
                                }
                                break;

                            default:
                                buf[pdata] = (byte)data;
                                pdata++;
                                break;
                        }
                    }
                    break;

                default:
                    break;
            }
            return retval;
        }
    }
}
