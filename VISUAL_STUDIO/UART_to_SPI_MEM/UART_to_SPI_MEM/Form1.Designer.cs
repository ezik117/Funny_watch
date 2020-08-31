namespace UART_to_SPI_MEM
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
            this.panel1 = new System.Windows.Forms.Panel();
            this.panel11 = new System.Windows.Forms.Panel();
            this.btnDisableWrite = new System.Windows.Forms.Button();
            this.btnClearLog = new System.Windows.Forms.Button();
            this.tbSerSpeed = new System.Windows.Forms.TextBox();
            this.panel10 = new System.Windows.Forms.Panel();
            this.btnReadManufID = new System.Windows.Forms.Button();
            this.label6 = new System.Windows.Forms.Label();
            this.cbPort = new System.Windows.Forms.ComboBox();
            this.panel9 = new System.Windows.Forms.Panel();
            this.btnReadMemoryToFile = new System.Windows.Forms.Button();
            this.panel5 = new System.Windows.Forms.Panel();
            this.label4 = new System.Windows.Forms.Label();
            this.tbWriteFromAddr = new System.Windows.Forms.TextBox();
            this.btnWriteFile = new System.Windows.Forms.Button();
            this.panel4 = new System.Windows.Forms.Panel();
            this.panel3 = new System.Windows.Forms.Panel();
            this.panel2 = new System.Windows.Forms.Panel();
            this.btnEraseChip = new System.Windows.Forms.Button();
            this.label3 = new System.Windows.Forms.Label();
            this.tbASector = new System.Windows.Forms.TextBox();
            this.btnEraseSector = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.tbAR2 = new System.Windows.Forms.TextBox();
            this.tbAR1 = new System.Windows.Forms.TextBox();
            this.btnReadMemoryToText = new System.Windows.Forms.Button();
            this.panel6 = new System.Windows.Forms.Panel();
            this.panel8 = new System.Windows.Forms.Panel();
            this.stat = new System.Windows.Forms.TextBox();
            this.panel7 = new System.Windows.Forms.Panel();
            this.label5 = new System.Windows.Forms.Label();
            this.pb = new System.Windows.Forms.ProgressBar();
            this.panel1.SuspendLayout();
            this.panel6.SuspendLayout();
            this.panel8.SuspendLayout();
            this.panel7.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.panel11);
            this.panel1.Controls.Add(this.btnDisableWrite);
            this.panel1.Controls.Add(this.btnClearLog);
            this.panel1.Controls.Add(this.tbSerSpeed);
            this.panel1.Controls.Add(this.panel10);
            this.panel1.Controls.Add(this.btnReadManufID);
            this.panel1.Controls.Add(this.label6);
            this.panel1.Controls.Add(this.cbPort);
            this.panel1.Controls.Add(this.panel9);
            this.panel1.Controls.Add(this.btnReadMemoryToFile);
            this.panel1.Controls.Add(this.panel5);
            this.panel1.Controls.Add(this.label4);
            this.panel1.Controls.Add(this.tbWriteFromAddr);
            this.panel1.Controls.Add(this.btnWriteFile);
            this.panel1.Controls.Add(this.panel4);
            this.panel1.Controls.Add(this.panel3);
            this.panel1.Controls.Add(this.panel2);
            this.panel1.Controls.Add(this.btnEraseChip);
            this.panel1.Controls.Add(this.label3);
            this.panel1.Controls.Add(this.tbASector);
            this.panel1.Controls.Add(this.btnEraseSector);
            this.panel1.Controls.Add(this.label2);
            this.panel1.Controls.Add(this.label1);
            this.panel1.Controls.Add(this.tbAR2);
            this.panel1.Controls.Add(this.tbAR1);
            this.panel1.Controls.Add(this.btnReadMemoryToText);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(339, 347);
            this.panel1.TabIndex = 0;
            // 
            // panel11
            // 
            this.panel11.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel11.Location = new System.Drawing.Point(8, 297);
            this.panel11.Name = "panel11";
            this.panel11.Size = new System.Drawing.Size(301, 4);
            this.panel11.TabIndex = 12;
            // 
            // btnDisableWrite
            // 
            this.btnDisableWrite.Location = new System.Drawing.Point(3, 268);
            this.btnDisableWrite.Name = "btnDisableWrite";
            this.btnDisableWrite.Size = new System.Drawing.Size(147, 23);
            this.btnDisableWrite.TabIndex = 21;
            this.btnDisableWrite.Text = "Disable Write";
            this.btnDisableWrite.UseVisualStyleBackColor = true;
            this.btnDisableWrite.Click += new System.EventHandler(this.btnDisableWrite_Click);
            // 
            // btnClearLog
            // 
            this.btnClearLog.Location = new System.Drawing.Point(227, 321);
            this.btnClearLog.Name = "btnClearLog";
            this.btnClearLog.Size = new System.Drawing.Size(91, 23);
            this.btnClearLog.TabIndex = 20;
            this.btnClearLog.Text = "Clear Log >";
            this.btnClearLog.UseVisualStyleBackColor = true;
            this.btnClearLog.Click += new System.EventHandler(this.btnClearLog_Click);
            // 
            // tbSerSpeed
            // 
            this.tbSerSpeed.Location = new System.Drawing.Point(156, 15);
            this.tbSerSpeed.Name = "tbSerSpeed";
            this.tbSerSpeed.Size = new System.Drawing.Size(100, 20);
            this.tbSerSpeed.TabIndex = 19;
            this.tbSerSpeed.Text = "115566";
            // 
            // panel10
            // 
            this.panel10.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel10.Location = new System.Drawing.Point(8, 82);
            this.panel10.Name = "panel10";
            this.panel10.Size = new System.Drawing.Size(301, 4);
            this.panel10.TabIndex = 18;
            // 
            // btnReadManufID
            // 
            this.btnReadManufID.Location = new System.Drawing.Point(3, 53);
            this.btnReadManufID.Name = "btnReadManufID";
            this.btnReadManufID.Size = new System.Drawing.Size(147, 23);
            this.btnReadManufID.TabIndex = 17;
            this.btnReadManufID.Text = "Read Manufacturer ID";
            this.btnReadManufID.UseVisualStyleBackColor = true;
            this.btnReadManufID.Click += new System.EventHandler(this.btnReadManufID_Click);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(12, 18);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(29, 13);
            this.label6.TabIndex = 16;
            this.label6.Text = "Port:";
            // 
            // cbPort
            // 
            this.cbPort.FormattingEnabled = true;
            this.cbPort.Items.AddRange(new object[] {
            "COM1",
            "COM2",
            "COM3",
            "COM4",
            "COM5",
            "COM6",
            "COM7",
            "COM8",
            "COM9"});
            this.cbPort.Location = new System.Drawing.Point(47, 15);
            this.cbPort.Name = "cbPort";
            this.cbPort.Size = new System.Drawing.Size(103, 21);
            this.cbPort.Sorted = true;
            this.cbPort.TabIndex = 15;
            // 
            // panel9
            // 
            this.panel9.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel9.Location = new System.Drawing.Point(8, 43);
            this.panel9.Name = "panel9";
            this.panel9.Size = new System.Drawing.Size(301, 4);
            this.panel9.TabIndex = 10;
            // 
            // btnReadMemoryToFile
            // 
            this.btnReadMemoryToFile.Location = new System.Drawing.Point(3, 115);
            this.btnReadMemoryToFile.Name = "btnReadMemoryToFile";
            this.btnReadMemoryToFile.Size = new System.Drawing.Size(147, 23);
            this.btnReadMemoryToFile.TabIndex = 14;
            this.btnReadMemoryToFile.Text = "Read Memory to File";
            this.btnReadMemoryToFile.UseVisualStyleBackColor = true;
            this.btnReadMemoryToFile.Click += new System.EventHandler(this.btnReadMemoryToFile_Click);
            // 
            // panel5
            // 
            this.panel5.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel5.Location = new System.Drawing.Point(8, 258);
            this.panel5.Name = "panel5";
            this.panel5.Size = new System.Drawing.Size(301, 4);
            this.panel5.TabIndex = 11;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(153, 234);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(71, 13);
            this.label4.TabIndex = 13;
            this.label4.Text = "From page 0x";
            // 
            // tbWriteFromAddr
            // 
            this.tbWriteFromAddr.Location = new System.Drawing.Point(227, 232);
            this.tbWriteFromAddr.Name = "tbWriteFromAddr";
            this.tbWriteFromAddr.Size = new System.Drawing.Size(100, 20);
            this.tbWriteFromAddr.TabIndex = 12;
            this.tbWriteFromAddr.Text = "000000";
            // 
            // btnWriteFile
            // 
            this.btnWriteFile.Location = new System.Drawing.Point(3, 229);
            this.btnWriteFile.Name = "btnWriteFile";
            this.btnWriteFile.Size = new System.Drawing.Size(147, 23);
            this.btnWriteFile.TabIndex = 11;
            this.btnWriteFile.Text = "Write File";
            this.btnWriteFile.UseVisualStyleBackColor = true;
            this.btnWriteFile.Click += new System.EventHandler(this.btnWriteFile_Click);
            // 
            // panel4
            // 
            this.panel4.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel4.Location = new System.Drawing.Point(8, 219);
            this.panel4.Name = "panel4";
            this.panel4.Size = new System.Drawing.Size(301, 4);
            this.panel4.TabIndex = 10;
            // 
            // panel3
            // 
            this.panel3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel3.Location = new System.Drawing.Point(8, 180);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(301, 4);
            this.panel3.TabIndex = 9;
            // 
            // panel2
            // 
            this.panel2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.panel2.Location = new System.Drawing.Point(8, 141);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(301, 4);
            this.panel2.TabIndex = 8;
            // 
            // btnEraseChip
            // 
            this.btnEraseChip.Location = new System.Drawing.Point(3, 190);
            this.btnEraseChip.Name = "btnEraseChip";
            this.btnEraseChip.Size = new System.Drawing.Size(147, 23);
            this.btnEraseChip.TabIndex = 6;
            this.btnEraseChip.Text = "Erase Chip";
            this.btnEraseChip.UseVisualStyleBackColor = true;
            this.btnEraseChip.Click += new System.EventHandler(this.btnEraseChip_Click);
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(172, 156);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(52, 13);
            this.label3.TabIndex = 7;
            this.label3.Text = "Sector 0x";
            // 
            // tbASector
            // 
            this.tbASector.Location = new System.Drawing.Point(227, 153);
            this.tbASector.Name = "tbASector";
            this.tbASector.Size = new System.Drawing.Size(100, 20);
            this.tbASector.TabIndex = 6;
            this.tbASector.Text = "000000";
            // 
            // btnEraseSector
            // 
            this.btnEraseSector.Location = new System.Drawing.Point(3, 151);
            this.btnEraseSector.Name = "btnEraseSector";
            this.btnEraseSector.Size = new System.Drawing.Size(147, 23);
            this.btnEraseSector.TabIndex = 5;
            this.btnEraseSector.Text = "Erase Sector";
            this.btnEraseSector.UseVisualStyleBackColor = true;
            this.btnEraseSector.Click += new System.EventHandler(this.btnEraseSector_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
            this.label2.Location = new System.Drawing.Point(166, 118);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(58, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "To addr 0x";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(156, 92);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(68, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "From addr 0x";
            // 
            // tbAR2
            // 
            this.tbAR2.Location = new System.Drawing.Point(227, 115);
            this.tbAR2.Name = "tbAR2";
            this.tbAR2.Size = new System.Drawing.Size(100, 20);
            this.tbAR2.TabIndex = 2;
            this.tbAR2.Text = "0000FF";
            // 
            // tbAR1
            // 
            this.tbAR1.Location = new System.Drawing.Point(227, 89);
            this.tbAR1.Name = "tbAR1";
            this.tbAR1.Size = new System.Drawing.Size(100, 20);
            this.tbAR1.TabIndex = 1;
            this.tbAR1.Text = "000000";
            // 
            // btnReadMemoryToText
            // 
            this.btnReadMemoryToText.Location = new System.Drawing.Point(3, 89);
            this.btnReadMemoryToText.Name = "btnReadMemoryToText";
            this.btnReadMemoryToText.Size = new System.Drawing.Size(147, 23);
            this.btnReadMemoryToText.TabIndex = 0;
            this.btnReadMemoryToText.Text = "Read Memory to Text";
            this.btnReadMemoryToText.UseVisualStyleBackColor = true;
            this.btnReadMemoryToText.Click += new System.EventHandler(this.btnReadMemoryToText_Click);
            // 
            // panel6
            // 
            this.panel6.Controls.Add(this.panel8);
            this.panel6.Controls.Add(this.panel7);
            this.panel6.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel6.Location = new System.Drawing.Point(339, 0);
            this.panel6.Name = "panel6";
            this.panel6.Size = new System.Drawing.Size(609, 347);
            this.panel6.TabIndex = 1;
            // 
            // panel8
            // 
            this.panel8.Controls.Add(this.stat);
            this.panel8.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel8.Location = new System.Drawing.Point(0, 53);
            this.panel8.Name = "panel8";
            this.panel8.Padding = new System.Windows.Forms.Padding(5);
            this.panel8.Size = new System.Drawing.Size(609, 294);
            this.panel8.TabIndex = 3;
            // 
            // stat
            // 
            this.stat.Dock = System.Windows.Forms.DockStyle.Fill;
            this.stat.Font = new System.Drawing.Font("Lucida Console", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.stat.Location = new System.Drawing.Point(5, 5);
            this.stat.Multiline = true;
            this.stat.Name = "stat";
            this.stat.ReadOnly = true;
            this.stat.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.stat.Size = new System.Drawing.Size(599, 284);
            this.stat.TabIndex = 0;
            // 
            // panel7
            // 
            this.panel7.Controls.Add(this.label5);
            this.panel7.Controls.Add(this.pb);
            this.panel7.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel7.Location = new System.Drawing.Point(0, 0);
            this.panel7.Margin = new System.Windows.Forms.Padding(0);
            this.panel7.Name = "panel7";
            this.panel7.Padding = new System.Windows.Forms.Padding(5);
            this.panel7.Size = new System.Drawing.Size(609, 53);
            this.panel7.TabIndex = 2;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(8, 10);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(48, 13);
            this.label5.TabIndex = 3;
            this.label5.Text = "Progress";
            // 
            // pb
            // 
            this.pb.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.pb.Location = new System.Drawing.Point(5, 25);
            this.pb.Maximum = 256;
            this.pb.Name = "pb";
            this.pb.Size = new System.Drawing.Size(599, 23);
            this.pb.TabIndex = 2;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(948, 347);
            this.Controls.Add(this.panel6);
            this.Controls.Add(this.panel1);
            this.Name = "Form1";
            this.Text = "ПРОГРАММАТОР WINBOND W25Q64FV";
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.panel6.ResumeLayout(false);
            this.panel8.ResumeLayout(false);
            this.panel8.PerformLayout();
            this.panel7.ResumeLayout(false);
            this.panel7.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.TextBox tbAR2;
        private System.Windows.Forms.TextBox tbAR1;
        private System.Windows.Forms.Button btnReadMemoryToText;
        private System.Windows.Forms.Panel panel5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.TextBox tbWriteFromAddr;
        private System.Windows.Forms.Button btnWriteFile;
        private System.Windows.Forms.Panel panel4;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Button btnEraseChip;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox tbASector;
        private System.Windows.Forms.Button btnEraseSector;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Panel panel6;
        private System.Windows.Forms.Panel panel7;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ProgressBar pb;
        private System.Windows.Forms.Panel panel8;
        private System.Windows.Forms.TextBox stat;
        private System.Windows.Forms.Button btnReadMemoryToFile;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox cbPort;
        private System.Windows.Forms.Panel panel9;
        private System.Windows.Forms.Panel panel10;
        private System.Windows.Forms.Button btnReadManufID;
        private System.Windows.Forms.TextBox tbSerSpeed;
        private System.Windows.Forms.Button btnClearLog;
        private System.Windows.Forms.Panel panel11;
        private System.Windows.Forms.Button btnDisableWrite;
    }
}

