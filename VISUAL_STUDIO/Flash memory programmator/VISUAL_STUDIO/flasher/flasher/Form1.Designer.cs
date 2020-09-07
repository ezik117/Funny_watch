namespace flasher
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.cbComPort = new System.Windows.Forms.ComboBox();
            this.btnComDisconnect = new System.Windows.Forms.Button();
            this.tbComSpeed = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.btnComConnect = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.btnAbort = new System.Windows.Forms.Button();
            this.pb = new System.Windows.Forms.ProgressBar();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.cbFillPagesAuto = new System.Windows.Forms.CheckBox();
            this.tbFillPages = new System.Windows.Forms.TextBox();
            this.btnFillPages = new System.Windows.Forms.Button();
            this.btnClearMem = new System.Windows.Forms.Button();
            this.btnWriteMemFromFile = new System.Windows.Forms.Button();
            this.btnRedMemToFile = new System.Windows.Forms.Button();
            this.btnEraseWholeMem = new System.Windows.Forms.Button();
            this.btnGetChipInfo = new System.Windows.Forms.Button();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.tbMessages = new System.Windows.Forms.TextBox();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.lblPages = new System.Windows.Forms.Label();
            this.lblSectors = new System.Windows.Forms.Label();
            this.lblStartSector = new System.Windows.Forms.Label();
            this.tbAddressLength = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.tbAddressStart = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.groupBox7 = new System.Windows.Forms.GroupBox();
            this.label5 = new System.Windows.Forms.Label();
            this.btnReadByte = new System.Windows.Forms.Button();
            this.tbByteToRW = new System.Windows.Forms.TextBox();
            this.btnWriteByte = new System.Windows.Forms.Button();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.btnSendByte = new System.Windows.Forms.Button();
            this.tbByteToSend = new System.Windows.Forms.TextBox();
            this.tbByteToRead1 = new System.Windows.Forms.TextBox();
            this.lblStatus = new System.Windows.Forms.Label();
            this.btnCS1 = new System.Windows.Forms.Button();
            this.tbByteToRead2 = new System.Windows.Forms.TextBox();
            this.btnCS0 = new System.Windows.Forms.Button();
            this.cbEmulate = new System.Windows.Forms.CheckBox();
            this.groupBox1.SuspendLayout();
            this.panel1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.groupBox5.SuspendLayout();
            this.groupBox7.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.cbEmulate);
            this.groupBox1.Controls.Add(this.cbComPort);
            this.groupBox1.Controls.Add(this.btnComDisconnect);
            this.groupBox1.Controls.Add(this.tbComSpeed);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.btnComConnect);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(13, 13);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(193, 158);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Connection";
            // 
            // cbComPort
            // 
            this.cbComPort.FormattingEnabled = true;
            this.cbComPort.Location = new System.Drawing.Point(83, 23);
            this.cbComPort.Name = "cbComPort";
            this.cbComPort.Size = new System.Drawing.Size(99, 21);
            this.cbComPort.TabIndex = 6;
            // 
            // btnComDisconnect
            // 
            this.btnComDisconnect.Enabled = false;
            this.btnComDisconnect.Location = new System.Drawing.Point(27, 122);
            this.btnComDisconnect.Name = "btnComDisconnect";
            this.btnComDisconnect.Size = new System.Drawing.Size(75, 23);
            this.btnComDisconnect.TabIndex = 5;
            this.btnComDisconnect.Text = "Disonnect";
            this.btnComDisconnect.UseVisualStyleBackColor = true;
            this.btnComDisconnect.Click += new System.EventHandler(this.btnComDisconnect_Click);
            // 
            // tbComSpeed
            // 
            this.tbComSpeed.Location = new System.Drawing.Point(82, 53);
            this.tbComSpeed.Name = "tbComSpeed";
            this.tbComSpeed.Size = new System.Drawing.Size(100, 20);
            this.tbComSpeed.TabIndex = 4;
            this.tbComSpeed.Text = "9600";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(24, 60);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(38, 13);
            this.label2.TabIndex = 3;
            this.label2.Text = "Speed";
            // 
            // btnComConnect
            // 
            this.btnComConnect.Location = new System.Drawing.Point(27, 93);
            this.btnComConnect.Name = "btnComConnect";
            this.btnComConnect.Size = new System.Drawing.Size(75, 23);
            this.btnComConnect.TabIndex = 1;
            this.btnComConnect.Text = "Connect";
            this.btnComConnect.UseVisualStyleBackColor = true;
            this.btnComConnect.Click += new System.EventHandler(this.btnComConnect_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(24, 32);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(52, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "COM port";
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.btnAbort);
            this.panel1.Controls.Add(this.pb);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(0, 419);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(800, 31);
            this.panel1.TabIndex = 1;
            // 
            // btnAbort
            // 
            this.btnAbort.Enabled = false;
            this.btnAbort.Location = new System.Drawing.Point(722, 5);
            this.btnAbort.Name = "btnAbort";
            this.btnAbort.Size = new System.Drawing.Size(75, 23);
            this.btnAbort.TabIndex = 7;
            this.btnAbort.Text = "Abort";
            this.btnAbort.UseVisualStyleBackColor = true;
            this.btnAbort.Click += new System.EventHandler(this.btnAbort_Click);
            // 
            // pb
            // 
            this.pb.Location = new System.Drawing.Point(12, 4);
            this.pb.Name = "pb";
            this.pb.Size = new System.Drawing.Size(704, 23);
            this.pb.TabIndex = 0;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.cbFillPagesAuto);
            this.groupBox2.Controls.Add(this.tbFillPages);
            this.groupBox2.Controls.Add(this.btnFillPages);
            this.groupBox2.Controls.Add(this.btnClearMem);
            this.groupBox2.Controls.Add(this.btnWriteMemFromFile);
            this.groupBox2.Controls.Add(this.btnRedMemToFile);
            this.groupBox2.Controls.Add(this.btnEraseWholeMem);
            this.groupBox2.Controls.Add(this.btnGetChipInfo);
            this.groupBox2.Location = new System.Drawing.Point(13, 178);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(193, 235);
            this.groupBox2.TabIndex = 2;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Operations";
            // 
            // cbFillPagesAuto
            // 
            this.cbFillPagesAuto.AutoSize = true;
            this.cbFillPagesAuto.Location = new System.Drawing.Point(104, 208);
            this.cbFillPagesAuto.Name = "cbFillPagesAuto";
            this.cbFillPagesAuto.Size = new System.Drawing.Size(47, 17);
            this.cbFillPagesAuto.TabIndex = 14;
            this.cbFillPagesAuto.Text = "auto";
            this.cbFillPagesAuto.UseVisualStyleBackColor = true;
            this.cbFillPagesAuto.CheckStateChanged += new System.EventHandler(this.cbFillPagesAuto_CheckStateChanged);
            // 
            // tbFillPages
            // 
            this.tbFillPages.Location = new System.Drawing.Point(152, 206);
            this.tbFillPages.Name = "tbFillPages";
            this.tbFillPages.Size = new System.Drawing.Size(30, 20);
            this.tbFillPages.TabIndex = 13;
            this.tbFillPages.Text = "0";
            // 
            // btnFillPages
            // 
            this.btnFillPages.Location = new System.Drawing.Point(27, 204);
            this.btnFillPages.Name = "btnFillPages";
            this.btnFillPages.Size = new System.Drawing.Size(75, 23);
            this.btnFillPages.TabIndex = 12;
            this.btnFillPages.Text = "Fill pages";
            this.btnFillPages.UseVisualStyleBackColor = true;
            this.btnFillPages.Click += new System.EventHandler(this.btnFillPages_Click);
            // 
            // btnClearMem
            // 
            this.btnClearMem.Location = new System.Drawing.Point(27, 151);
            this.btnClearMem.Name = "btnClearMem";
            this.btnClearMem.Size = new System.Drawing.Size(155, 23);
            this.btnClearMem.TabIndex = 11;
            this.btnClearMem.Text = "Wipe selected memory";
            this.btnClearMem.UseVisualStyleBackColor = true;
            this.btnClearMem.Click += new System.EventHandler(this.btnClearMem_Click);
            // 
            // btnWriteMemFromFile
            // 
            this.btnWriteMemFromFile.Location = new System.Drawing.Point(27, 122);
            this.btnWriteMemFromFile.Name = "btnWriteMemFromFile";
            this.btnWriteMemFromFile.Size = new System.Drawing.Size(155, 23);
            this.btnWriteMemFromFile.TabIndex = 10;
            this.btnWriteMemFromFile.Text = "Write memory from file";
            this.btnWriteMemFromFile.UseVisualStyleBackColor = true;
            this.btnWriteMemFromFile.Click += new System.EventHandler(this.btnWriteMemFromFile_Click);
            // 
            // btnRedMemToFile
            // 
            this.btnRedMemToFile.Location = new System.Drawing.Point(27, 93);
            this.btnRedMemToFile.Name = "btnRedMemToFile";
            this.btnRedMemToFile.Size = new System.Drawing.Size(155, 23);
            this.btnRedMemToFile.TabIndex = 9;
            this.btnRedMemToFile.Text = "Read memory to file";
            this.btnRedMemToFile.UseVisualStyleBackColor = true;
            this.btnRedMemToFile.Click += new System.EventHandler(this.btnRedMemToFile_Click);
            // 
            // btnEraseWholeMem
            // 
            this.btnEraseWholeMem.Location = new System.Drawing.Point(27, 51);
            this.btnEraseWholeMem.Name = "btnEraseWholeMem";
            this.btnEraseWholeMem.Size = new System.Drawing.Size(155, 23);
            this.btnEraseWholeMem.TabIndex = 8;
            this.btnEraseWholeMem.Text = "Erase whole memory";
            this.btnEraseWholeMem.UseVisualStyleBackColor = true;
            this.btnEraseWholeMem.Click += new System.EventHandler(this.btnEraseWholeMem_Click);
            // 
            // btnGetChipInfo
            // 
            this.btnGetChipInfo.Location = new System.Drawing.Point(27, 22);
            this.btnGetChipInfo.Name = "btnGetChipInfo";
            this.btnGetChipInfo.Size = new System.Drawing.Size(155, 23);
            this.btnGetChipInfo.TabIndex = 7;
            this.btnGetChipInfo.Text = "Get chip info";
            this.btnGetChipInfo.UseVisualStyleBackColor = true;
            this.btnGetChipInfo.Click += new System.EventHandler(this.btnGetChipInfo_Click);
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.tbMessages);
            this.groupBox3.Location = new System.Drawing.Point(212, 178);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(588, 235);
            this.groupBox3.TabIndex = 3;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Messages";
            // 
            // tbMessages
            // 
            this.tbMessages.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(0)))), ((int)(((byte)(192)))));
            this.tbMessages.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tbMessages.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.tbMessages.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.tbMessages.Location = new System.Drawing.Point(3, 16);
            this.tbMessages.Multiline = true;
            this.tbMessages.Name = "tbMessages";
            this.tbMessages.ReadOnly = true;
            this.tbMessages.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbMessages.Size = new System.Drawing.Size(582, 216);
            this.tbMessages.TabIndex = 5;
            this.tbMessages.MouseDown += new System.Windows.Forms.MouseEventHandler(this.tbMessages_MouseDown);
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.lblPages);
            this.groupBox4.Controls.Add(this.lblSectors);
            this.groupBox4.Controls.Add(this.lblStartSector);
            this.groupBox4.Controls.Add(this.tbAddressLength);
            this.groupBox4.Controls.Add(this.label4);
            this.groupBox4.Controls.Add(this.tbAddressStart);
            this.groupBox4.Controls.Add(this.label3);
            this.groupBox4.Location = new System.Drawing.Point(212, 13);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(200, 158);
            this.groupBox4.TabIndex = 4;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Addressing";
            // 
            // lblPages
            // 
            this.lblPages.AutoSize = true;
            this.lblPages.Location = new System.Drawing.Point(129, 132);
            this.lblPages.Name = "lblPages";
            this.lblPages.Size = new System.Drawing.Size(49, 13);
            this.lblPages.TabIndex = 10;
            this.lblPages.Text = "Pages: 0";
            // 
            // lblSectors
            // 
            this.lblSectors.AutoSize = true;
            this.lblSectors.Location = new System.Drawing.Point(24, 132);
            this.lblSectors.Name = "lblSectors";
            this.lblSectors.Size = new System.Drawing.Size(55, 13);
            this.lblSectors.TabIndex = 9;
            this.lblSectors.Text = "Sectors: 0";
            // 
            // lblStartSector
            // 
            this.lblStartSector.AutoSize = true;
            this.lblStartSector.Location = new System.Drawing.Point(6, 113);
            this.lblStartSector.Name = "lblStartSector";
            this.lblStartSector.Size = new System.Drawing.Size(73, 13);
            this.lblStartSector.TabIndex = 8;
            this.lblStartSector.Text = "Start sector: 0";
            // 
            // tbAddressLength
            // 
            this.tbAddressLength.Location = new System.Drawing.Point(13, 78);
            this.tbAddressLength.Name = "tbAddressLength";
            this.tbAddressLength.Size = new System.Drawing.Size(181, 20);
            this.tbAddressLength.TabIndex = 7;
            this.tbAddressLength.Text = "1";
            this.tbAddressLength.TextChanged += new System.EventHandler(this.tbAddressStart_TextChanged);
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(10, 62);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(71, 13);
            this.label4.TabIndex = 6;
            this.label4.Text = "Length, bytes";
            // 
            // tbAddressStart
            // 
            this.tbAddressStart.Location = new System.Drawing.Point(13, 39);
            this.tbAddressStart.Name = "tbAddressStart";
            this.tbAddressStart.Size = new System.Drawing.Size(181, 20);
            this.tbAddressStart.TabIndex = 5;
            this.tbAddressStart.Text = "0";
            this.tbAddressStart.TextChanged += new System.EventHandler(this.tbAddressStart_TextChanged);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(10, 23);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(111, 13);
            this.label3.TabIndex = 4;
            this.label3.Text = "Start address, decimal";
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.groupBox7);
            this.groupBox5.Controls.Add(this.groupBox6);
            this.groupBox5.Location = new System.Drawing.Point(418, 13);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Size = new System.Drawing.Size(370, 158);
            this.groupBox5.TabIndex = 5;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "Direct operate";
            // 
            // groupBox7
            // 
            this.groupBox7.Controls.Add(this.label5);
            this.groupBox7.Controls.Add(this.btnReadByte);
            this.groupBox7.Controls.Add(this.tbByteToRW);
            this.groupBox7.Controls.Add(this.btnWriteByte);
            this.groupBox7.Location = new System.Drawing.Point(219, 19);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Size = new System.Drawing.Size(135, 129);
            this.groupBox7.TabIndex = 17;
            this.groupBox7.TabStop = false;
            this.groupBox7.Text = "Direct memory access";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(85, 29);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(27, 13);
            this.label5.TabIndex = 16;
            this.label5.Text = "byte";
            // 
            // btnReadByte
            // 
            this.btnReadByte.Location = new System.Drawing.Point(8, 56);
            this.btnReadByte.Name = "btnReadByte";
            this.btnReadByte.Size = new System.Drawing.Size(75, 23);
            this.btnReadByte.TabIndex = 13;
            this.btnReadByte.Text = "Read byte";
            this.btnReadByte.UseVisualStyleBackColor = true;
            this.btnReadByte.Click += new System.EventHandler(this.btnReadByte_Click);
            // 
            // tbByteToRW
            // 
            this.tbByteToRW.Location = new System.Drawing.Point(8, 25);
            this.tbByteToRW.Name = "tbByteToRW";
            this.tbByteToRW.Size = new System.Drawing.Size(75, 20);
            this.tbByteToRW.TabIndex = 15;
            this.tbByteToRW.Text = "0";
            // 
            // btnWriteByte
            // 
            this.btnWriteByte.Location = new System.Drawing.Point(8, 85);
            this.btnWriteByte.Name = "btnWriteByte";
            this.btnWriteByte.Size = new System.Drawing.Size(75, 23);
            this.btnWriteByte.TabIndex = 14;
            this.btnWriteByte.Text = "Write byte";
            this.btnWriteByte.UseVisualStyleBackColor = true;
            this.btnWriteByte.Click += new System.EventHandler(this.btnWriteByte_Click);
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.btnSendByte);
            this.groupBox6.Controls.Add(this.tbByteToSend);
            this.groupBox6.Controls.Add(this.tbByteToRead1);
            this.groupBox6.Controls.Add(this.lblStatus);
            this.groupBox6.Controls.Add(this.btnCS1);
            this.groupBox6.Controls.Add(this.tbByteToRead2);
            this.groupBox6.Controls.Add(this.btnCS0);
            this.groupBox6.Location = new System.Drawing.Point(6, 19);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(207, 129);
            this.groupBox6.TabIndex = 16;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Dongle data exchange";
            // 
            // btnSendByte
            // 
            this.btnSendByte.Location = new System.Drawing.Point(118, 26);
            this.btnSendByte.Name = "btnSendByte";
            this.btnSendByte.Size = new System.Drawing.Size(75, 23);
            this.btnSendByte.TabIndex = 7;
            this.btnSendByte.Text = "Send byte";
            this.btnSendByte.UseVisualStyleBackColor = true;
            this.btnSendByte.Click += new System.EventHandler(this.btnSendByte_Click);
            // 
            // tbByteToSend
            // 
            this.tbByteToSend.Location = new System.Drawing.Point(12, 29);
            this.tbByteToSend.Name = "tbByteToSend";
            this.tbByteToSend.Size = new System.Drawing.Size(100, 20);
            this.tbByteToSend.TabIndex = 5;
            this.tbByteToSend.Text = "0";
            // 
            // tbByteToRead1
            // 
            this.tbByteToRead1.Location = new System.Drawing.Point(12, 56);
            this.tbByteToRead1.Name = "tbByteToRead1";
            this.tbByteToRead1.ReadOnly = true;
            this.tbByteToRead1.Size = new System.Drawing.Size(46, 20);
            this.tbByteToRead1.TabIndex = 8;
            this.tbByteToRead1.Text = "0";
            // 
            // lblStatus
            // 
            this.lblStatus.AutoSize = true;
            this.lblStatus.Location = new System.Drawing.Point(118, 63);
            this.lblStatus.Name = "lblStatus";
            this.lblStatus.Size = new System.Drawing.Size(62, 13);
            this.lblStatus.TabIndex = 7;
            this.lblStatus.Text = "<STATUS>";
            // 
            // btnCS1
            // 
            this.btnCS1.Location = new System.Drawing.Point(12, 95);
            this.btnCS1.Name = "btnCS1";
            this.btnCS1.Size = new System.Drawing.Size(54, 23);
            this.btnCS1.TabIndex = 9;
            this.btnCS1.Text = "/CS=0";
            this.btnCS1.UseVisualStyleBackColor = true;
            this.btnCS1.Click += new System.EventHandler(this.btnCS1_Click);
            // 
            // tbByteToRead2
            // 
            this.tbByteToRead2.Location = new System.Drawing.Point(66, 56);
            this.tbByteToRead2.Name = "tbByteToRead2";
            this.tbByteToRead2.ReadOnly = true;
            this.tbByteToRead2.Size = new System.Drawing.Size(46, 20);
            this.tbByteToRead2.TabIndex = 11;
            this.tbByteToRead2.Text = "0";
            // 
            // btnCS0
            // 
            this.btnCS0.Location = new System.Drawing.Point(66, 95);
            this.btnCS0.Name = "btnCS0";
            this.btnCS0.Size = new System.Drawing.Size(54, 23);
            this.btnCS0.TabIndex = 10;
            this.btnCS0.Text = "/CS=1";
            this.btnCS0.UseVisualStyleBackColor = true;
            this.btnCS0.Click += new System.EventHandler(this.btnCS0_Click);
            // 
            // cbEmulate
            // 
            this.cbEmulate.AutoSize = true;
            this.cbEmulate.Location = new System.Drawing.Point(123, 97);
            this.cbEmulate.Name = "cbEmulate";
            this.cbEmulate.Size = new System.Drawing.Size(64, 17);
            this.cbEmulate.TabIndex = 15;
            this.cbEmulate.Text = "Emulate";
            this.cbEmulate.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.groupBox5);
            this.Controls.Add(this.groupBox4);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.panel1);
            this.Controls.Add(this.groupBox1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.Name = "Form1";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "WINBOND W25Q64 FLASH UTILITY";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.panel1.ResumeLayout(false);
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            this.groupBox5.ResumeLayout(false);
            this.groupBox7.ResumeLayout(false);
            this.groupBox7.PerformLayout();
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button btnComConnect;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button btnComDisconnect;
        private System.Windows.Forms.TextBox tbComSpeed;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox cbComPort;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.ProgressBar pb;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Button btnRedMemToFile;
        private System.Windows.Forms.Button btnEraseWholeMem;
        private System.Windows.Forms.Button btnGetChipInfo;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.TextBox tbMessages;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Label lblSectors;
        private System.Windows.Forms.Label lblStartSector;
        private System.Windows.Forms.TextBox tbAddressLength;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox tbAddressStart;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Button btnClearMem;
        private System.Windows.Forms.Button btnWriteMemFromFile;
        private System.Windows.Forms.Button btnAbort;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.Button btnCS0;
        private System.Windows.Forms.Button btnCS1;
        private System.Windows.Forms.Label lblStatus;
        private System.Windows.Forms.TextBox tbByteToRead1;
        private System.Windows.Forms.Button btnSendByte;
        private System.Windows.Forms.TextBox tbByteToSend;
        private System.Windows.Forms.TextBox tbByteToRead2;
        private System.Windows.Forms.TextBox tbByteToRW;
        private System.Windows.Forms.Button btnWriteByte;
        private System.Windows.Forms.Button btnReadByte;
        private System.Windows.Forms.GroupBox groupBox7;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.TextBox tbFillPages;
        private System.Windows.Forms.Button btnFillPages;
        private System.Windows.Forms.CheckBox cbFillPagesAuto;
        private System.Windows.Forms.Label lblPages;
        private System.Windows.Forms.CheckBox cbEmulate;
    }
}

