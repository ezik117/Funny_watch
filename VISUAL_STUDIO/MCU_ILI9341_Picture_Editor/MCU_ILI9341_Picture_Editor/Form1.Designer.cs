namespace MCU_ILI9341_Picture_Editor
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
            this.btn_ConvertToByte = new System.Windows.Forms.Button();
            this.btnClear = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.rbYBT = new System.Windows.Forms.RadioButton();
            this.rbYTB = new System.Windows.Forms.RadioButton();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.rbXRL = new System.Windows.Forms.RadioButton();
            this.rbXLR = new System.Windows.Forms.RadioButton();
            this.btn_PB1_Convert = new System.Windows.Forms.Button();
            this.btn_PB1_Load = new System.Windows.Forms.Button();
            this.PB1 = new System.Windows.Forms.PictureBox();
            this.panel2 = new System.Windows.Forms.Panel();
            this.TB1 = new System.Windows.Forms.TextBox();
            this.tbFileName = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.cbToFileOnly = new System.Windows.Forms.CheckBox();
            this.panel1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.PB1)).BeginInit();
            this.panel2.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.cbToFileOnly);
            this.panel1.Controls.Add(this.label1);
            this.panel1.Controls.Add(this.tbFileName);
            this.panel1.Controls.Add(this.btn_ConvertToByte);
            this.panel1.Controls.Add(this.btnClear);
            this.panel1.Controls.Add(this.groupBox2);
            this.panel1.Controls.Add(this.groupBox1);
            this.panel1.Controls.Add(this.btn_PB1_Convert);
            this.panel1.Controls.Add(this.btn_PB1_Load);
            this.panel1.Controls.Add(this.PB1);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(437, 379);
            this.panel1.TabIndex = 1;
            // 
            // btn_ConvertToByte
            // 
            this.btn_ConvertToByte.Location = new System.Drawing.Point(331, 71);
            this.btn_ConvertToByte.Name = "btn_ConvertToByte";
            this.btn_ConvertToByte.Size = new System.Drawing.Size(89, 23);
            this.btn_ConvertToByte.TabIndex = 7;
            this.btn_ConvertToByte.Text = "Convert to byte";
            this.btn_ConvertToByte.UseVisualStyleBackColor = true;
            this.btn_ConvertToByte.Click += new System.EventHandler(this.btn_ConvertToByte_Click);
            // 
            // btnClear
            // 
            this.btnClear.Location = new System.Drawing.Point(331, 118);
            this.btnClear.Name = "btnClear";
            this.btnClear.Size = new System.Drawing.Size(89, 23);
            this.btnClear.TabIndex = 6;
            this.btnClear.Text = "Clear";
            this.btnClear.UseVisualStyleBackColor = true;
            this.btnClear.Click += new System.EventHandler(this.btnClear_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.rbYBT);
            this.groupBox2.Controls.Add(this.rbYTB);
            this.groupBox2.Location = new System.Drawing.Point(136, 252);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(126, 73);
            this.groupBox2.TabIndex = 5;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "сканирование Y";
            // 
            // rbYBT
            // 
            this.rbYBT.AutoSize = true;
            this.rbYBT.Location = new System.Drawing.Point(8, 43);
            this.rbYBT.Name = "rbYBT";
            this.rbYBT.Size = new System.Drawing.Size(86, 17);
            this.rbYBT.TabIndex = 1;
            this.rbYBT.Text = "снизу-вверх";
            this.rbYBT.UseVisualStyleBackColor = true;
            // 
            // rbYTB
            // 
            this.rbYTB.AutoSize = true;
            this.rbYTB.Checked = true;
            this.rbYTB.Location = new System.Drawing.Point(8, 20);
            this.rbYTB.Name = "rbYTB";
            this.rbYTB.Size = new System.Drawing.Size(86, 17);
            this.rbYTB.TabIndex = 0;
            this.rbYTB.TabStop = true;
            this.rbYTB.Text = "сверху-вниз";
            this.rbYTB.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.rbXRL);
            this.groupBox1.Controls.Add(this.rbXLR);
            this.groupBox1.Location = new System.Drawing.Point(4, 252);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(126, 73);
            this.groupBox1.TabIndex = 4;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "сканирование X";
            // 
            // rbXRL
            // 
            this.rbXRL.AutoSize = true;
            this.rbXRL.Location = new System.Drawing.Point(8, 43);
            this.rbXRL.Name = "rbXRL";
            this.rbXRL.Size = new System.Drawing.Size(100, 17);
            this.rbXRL.TabIndex = 1;
            this.rbXRL.Text = "справа-налево";
            this.rbXRL.UseVisualStyleBackColor = true;
            // 
            // rbXLR
            // 
            this.rbXLR.AutoSize = true;
            this.rbXLR.Checked = true;
            this.rbXLR.Location = new System.Drawing.Point(8, 20);
            this.rbXLR.Name = "rbXLR";
            this.rbXLR.Size = new System.Drawing.Size(100, 17);
            this.rbXLR.TabIndex = 0;
            this.rbXLR.TabStop = true;
            this.rbXLR.Text = "слева-направо";
            this.rbXLR.UseVisualStyleBackColor = true;
            // 
            // btn_PB1_Convert
            // 
            this.btn_PB1_Convert.Location = new System.Drawing.Point(330, 42);
            this.btn_PB1_Convert.Name = "btn_PB1_Convert";
            this.btn_PB1_Convert.Size = new System.Drawing.Size(90, 23);
            this.btn_PB1_Convert.TabIndex = 3;
            this.btn_PB1_Convert.Text = "Convert to px";
            this.btn_PB1_Convert.UseVisualStyleBackColor = true;
            this.btn_PB1_Convert.Click += new System.EventHandler(this.btn_PB1_Convert_Click);
            // 
            // btn_PB1_Load
            // 
            this.btn_PB1_Load.Location = new System.Drawing.Point(330, 13);
            this.btn_PB1_Load.Name = "btn_PB1_Load";
            this.btn_PB1_Load.Size = new System.Drawing.Size(90, 23);
            this.btn_PB1_Load.TabIndex = 2;
            this.btn_PB1_Load.Text = "Load";
            this.btn_PB1_Load.UseVisualStyleBackColor = true;
            this.btn_PB1_Load.Click += new System.EventHandler(this.btn_PB1_Load_Click);
            // 
            // PB1
            // 
            this.PB1.BackColor = System.Drawing.Color.White;
            this.PB1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.PB1.Location = new System.Drawing.Point(3, 3);
            this.PB1.Name = "PB1";
            this.PB1.Size = new System.Drawing.Size(322, 242);
            this.PB1.TabIndex = 1;
            this.PB1.TabStop = false;
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.TB1);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel2.Location = new System.Drawing.Point(437, 0);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(420, 379);
            this.panel2.TabIndex = 3;
            // 
            // TB1
            // 
            this.TB1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.TB1.Location = new System.Drawing.Point(0, 0);
            this.TB1.Multiline = true;
            this.TB1.Name = "TB1";
            this.TB1.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.TB1.Size = new System.Drawing.Size(420, 379);
            this.TB1.TabIndex = 3;
            this.TB1.WordWrap = false;
            // 
            // tbFileName
            // 
            this.tbFileName.Location = new System.Drawing.Point(4, 347);
            this.tbFileName.Name = "tbFileName";
            this.tbFileName.Size = new System.Drawing.Size(416, 20);
            this.tbFileName.TabIndex = 8;
            this.tbFileName.Text = "out";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(4, 328);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(150, 13);
            this.label1.TabIndex = 9;
            this.label1.Text = "Имя файла для сохранения:";
            // 
            // cbToFileOnly
            // 
            this.cbToFileOnly.AutoSize = true;
            this.cbToFileOnly.Location = new System.Drawing.Point(161, 328);
            this.cbToFileOnly.Name = "cbToFileOnly";
            this.cbToFileOnly.Size = new System.Drawing.Size(99, 17);
            this.cbToFileOnly.TabIndex = 10;
            this.cbToFileOnly.Text = "только в файл";
            this.cbToFileOnly.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(857, 379);
            this.Controls.Add(this.panel2);
            this.Controls.Add(this.panel1);
            this.Name = "Form1";
            this.Text = "КОНВЕРТОР РИСУНКА BMP В ФАЙЛ И ТЕКСТ";
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.PB1)).EndInit();
            this.panel2.ResumeLayout(false);
            this.panel2.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button btn_PB1_Load;
        private System.Windows.Forms.PictureBox PB1;
        private System.Windows.Forms.Button btn_PB1_Convert;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.RadioButton rbYBT;
        private System.Windows.Forms.RadioButton rbYTB;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.RadioButton rbXRL;
        private System.Windows.Forms.RadioButton rbXLR;
        private System.Windows.Forms.Button btnClear;
        private System.Windows.Forms.Button btn_ConvertToByte;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.TextBox TB1;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tbFileName;
        private System.Windows.Forms.CheckBox cbToFileOnly;
    }
}

