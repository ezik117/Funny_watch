namespace bmp_converter
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
            this.cbAddToExisted = new System.Windows.Forms.CheckBox();
            this.btnSaveImage = new System.Windows.Forms.Button();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.rbTextData = new System.Windows.Forms.RadioButton();
            this.rbBinaryData = new System.Windows.Forms.RadioButton();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.rb256ColorDots = new System.Windows.Forms.RadioButton();
            this.rbBlackWhiteDots = new System.Windows.Forms.RadioButton();
            this.rb16ColorDots = new System.Windows.Forms.RadioButton();
            this.rbFullColor = new System.Windows.Forms.RadioButton();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.panel6 = new System.Windows.Forms.Panel();
            this.rbBottomToTop = new System.Windows.Forms.RadioButton();
            this.rbTopToBottom = new System.Windows.Forms.RadioButton();
            this.panel5 = new System.Windows.Forms.Panel();
            this.rbRightToLeft = new System.Windows.Forms.RadioButton();
            this.rbLeftToRight = new System.Windows.Forms.RadioButton();
            this.btnLoad = new System.Windows.Forms.Button();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.tbInfo = new System.Windows.Forms.TextBox();
            this.pic = new System.Windows.Forms.PictureBox();
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.toolsToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuColorConvertor = new System.Windows.Forms.ToolStripMenuItem();
            this.panel1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.panel6.SuspendLayout();
            this.panel5.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pic)).BeginInit();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // panel1
            // 
            this.panel1.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.panel1.Controls.Add(this.cbAddToExisted);
            this.panel1.Controls.Add(this.btnSaveImage);
            this.panel1.Controls.Add(this.groupBox2);
            this.panel1.Controls.Add(this.groupBox3);
            this.panel1.Controls.Add(this.groupBox1);
            this.panel1.Controls.Add(this.btnLoad);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Right;
            this.panel1.Location = new System.Drawing.Point(600, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(200, 450);
            this.panel1.TabIndex = 0;
            // 
            // cbAddToExisted
            // 
            this.cbAddToExisted.AutoSize = true;
            this.cbAddToExisted.Location = new System.Drawing.Point(43, 396);
            this.cbAddToExisted.Name = "cbAddToExisted";
            this.cbAddToExisted.Size = new System.Drawing.Size(119, 17);
            this.cbAddToExisted.TabIndex = 5;
            this.cbAddToExisted.Text = "Добавить к файлу";
            this.cbAddToExisted.UseVisualStyleBackColor = true;
            // 
            // btnSaveImage
            // 
            this.btnSaveImage.Enabled = false;
            this.btnSaveImage.Location = new System.Drawing.Point(61, 414);
            this.btnSaveImage.Name = "btnSaveImage";
            this.btnSaveImage.Size = new System.Drawing.Size(75, 23);
            this.btnSaveImage.TabIndex = 5;
            this.btnSaveImage.Text = "Save image";
            this.btnSaveImage.UseVisualStyleBackColor = true;
            this.btnSaveImage.Click += new System.EventHandler(this.btnSaveImage_Click);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.rbTextData);
            this.groupBox2.Controls.Add(this.rbBinaryData);
            this.groupBox2.Location = new System.Drawing.Point(6, 319);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(181, 71);
            this.groupBox2.TabIndex = 4;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Формат вывода";
            // 
            // rbTextData
            // 
            this.rbTextData.AutoSize = true;
            this.rbTextData.Location = new System.Drawing.Point(24, 42);
            this.rbTextData.Name = "rbTextData";
            this.rbTextData.Size = new System.Drawing.Size(86, 17);
            this.rbTextData.TabIndex = 2;
            this.rbTextData.Text = "Текст (HEX)";
            this.rbTextData.UseVisualStyleBackColor = true;
            // 
            // rbBinaryData
            // 
            this.rbBinaryData.AutoSize = true;
            this.rbBinaryData.Checked = true;
            this.rbBinaryData.Location = new System.Drawing.Point(24, 19);
            this.rbBinaryData.Name = "rbBinaryData";
            this.rbBinaryData.Size = new System.Drawing.Size(118, 17);
            this.rbBinaryData.TabIndex = 1;
            this.rbBinaryData.TabStop = true;
            this.rbBinaryData.Text = "Двоичные данные";
            this.rbBinaryData.UseVisualStyleBackColor = true;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.rb256ColorDots);
            this.groupBox3.Controls.Add(this.rbBlackWhiteDots);
            this.groupBox3.Controls.Add(this.rb16ColorDots);
            this.groupBox3.Controls.Add(this.rbFullColor);
            this.groupBox3.Location = new System.Drawing.Point(6, 177);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(181, 136);
            this.groupBox3.TabIndex = 3;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "Сохранить как";
            // 
            // rb256ColorDots
            // 
            this.rb256ColorDots.AutoSize = true;
            this.rb256ColorDots.Enabled = false;
            this.rb256ColorDots.Location = new System.Drawing.Point(19, 41);
            this.rb256ColorDots.Name = "rb256ColorDots";
            this.rb256ColorDots.Size = new System.Drawing.Size(144, 17);
            this.rb256ColorDots.TabIndex = 5;
            this.rb256ColorDots.Text = "переменный 8-бит цвет";
            this.rb256ColorDots.UseVisualStyleBackColor = true;
            // 
            // rbBlackWhiteDots
            // 
            this.rbBlackWhiteDots.AutoSize = true;
            this.rbBlackWhiteDots.Location = new System.Drawing.Point(19, 87);
            this.rbBlackWhiteDots.Name = "rbBlackWhiteDots";
            this.rbBlackWhiteDots.Size = new System.Drawing.Size(102, 17);
            this.rbBlackWhiteDots.TabIndex = 4;
            this.rbBlackWhiteDots.Text = "1-бит цвет (ч\\б)";
            this.rbBlackWhiteDots.UseVisualStyleBackColor = true;
            // 
            // rb16ColorDots
            // 
            this.rb16ColorDots.AutoSize = true;
            this.rb16ColorDots.Location = new System.Drawing.Point(19, 64);
            this.rb16ColorDots.Name = "rb16ColorDots";
            this.rb16ColorDots.Size = new System.Drawing.Size(142, 17);
            this.rb16ColorDots.TabIndex = 3;
            this.rb16ColorDots.Text = "4-бит цвет (с палитрой)";
            this.rb16ColorDots.UseVisualStyleBackColor = true;
            // 
            // rbFullColor
            // 
            this.rbFullColor.AutoSize = true;
            this.rbFullColor.Checked = true;
            this.rbFullColor.Location = new System.Drawing.Point(19, 18);
            this.rbFullColor.Name = "rbFullColor";
            this.rbFullColor.Size = new System.Drawing.Size(156, 17);
            this.rbFullColor.TabIndex = 1;
            this.rbFullColor.TabStop = true;
            this.rbFullColor.Text = "16-бит цвет (без палитры)";
            this.rbFullColor.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.panel6);
            this.groupBox1.Controls.Add(this.panel5);
            this.groupBox1.Location = new System.Drawing.Point(6, 43);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(181, 128);
            this.groupBox1.TabIndex = 1;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Сканирование";
            // 
            // panel6
            // 
            this.panel6.Controls.Add(this.rbBottomToTop);
            this.panel6.Controls.Add(this.rbTopToBottom);
            this.panel6.Location = new System.Drawing.Point(7, 69);
            this.panel6.Name = "panel6";
            this.panel6.Size = new System.Drawing.Size(168, 53);
            this.panel6.TabIndex = 1;
            // 
            // rbBottomToTop
            // 
            this.rbBottomToTop.AutoSize = true;
            this.rbBottomToTop.Location = new System.Drawing.Point(17, 26);
            this.rbBottomToTop.Name = "rbBottomToTop";
            this.rbBottomToTop.Size = new System.Drawing.Size(86, 17);
            this.rbBottomToTop.TabIndex = 1;
            this.rbBottomToTop.Text = "снизу-вверх";
            this.rbBottomToTop.UseVisualStyleBackColor = true;
            // 
            // rbTopToBottom
            // 
            this.rbTopToBottom.AutoSize = true;
            this.rbTopToBottom.Checked = true;
            this.rbTopToBottom.Location = new System.Drawing.Point(17, 4);
            this.rbTopToBottom.Name = "rbTopToBottom";
            this.rbTopToBottom.Size = new System.Drawing.Size(86, 17);
            this.rbTopToBottom.TabIndex = 0;
            this.rbTopToBottom.TabStop = true;
            this.rbTopToBottom.Text = "сверху-вниз";
            this.rbTopToBottom.UseVisualStyleBackColor = true;
            // 
            // panel5
            // 
            this.panel5.Controls.Add(this.rbRightToLeft);
            this.panel5.Controls.Add(this.rbLeftToRight);
            this.panel5.Location = new System.Drawing.Point(7, 20);
            this.panel5.Name = "panel5";
            this.panel5.Size = new System.Drawing.Size(168, 53);
            this.panel5.TabIndex = 0;
            // 
            // rbRightToLeft
            // 
            this.rbRightToLeft.AutoSize = true;
            this.rbRightToLeft.Location = new System.Drawing.Point(17, 26);
            this.rbRightToLeft.Name = "rbRightToLeft";
            this.rbRightToLeft.Size = new System.Drawing.Size(100, 17);
            this.rbRightToLeft.TabIndex = 1;
            this.rbRightToLeft.Text = "справа-налево";
            this.rbRightToLeft.UseVisualStyleBackColor = true;
            // 
            // rbLeftToRight
            // 
            this.rbLeftToRight.AutoSize = true;
            this.rbLeftToRight.Checked = true;
            this.rbLeftToRight.Location = new System.Drawing.Point(17, 4);
            this.rbLeftToRight.Name = "rbLeftToRight";
            this.rbLeftToRight.Size = new System.Drawing.Size(100, 17);
            this.rbLeftToRight.TabIndex = 0;
            this.rbLeftToRight.TabStop = true;
            this.rbLeftToRight.Text = "слева-направо";
            this.rbLeftToRight.UseVisualStyleBackColor = true;
            // 
            // btnLoad
            // 
            this.btnLoad.Location = new System.Drawing.Point(61, 12);
            this.btnLoad.Name = "btnLoad";
            this.btnLoad.Size = new System.Drawing.Size(75, 23);
            this.btnLoad.TabIndex = 0;
            this.btnLoad.Text = "Load image";
            this.btnLoad.UseVisualStyleBackColor = true;
            this.btnLoad.Click += new System.EventHandler(this.btnLoad_Click);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.tbInfo);
            this.splitContainer1.Panel1.Controls.Add(this.pic);
            this.splitContainer1.Size = new System.Drawing.Size(600, 450);
            this.splitContainer1.SplitterDistance = 317;
            this.splitContainer1.TabIndex = 2;
            // 
            // tbInfo
            // 
            this.tbInfo.BackColor = System.Drawing.Color.DimGray;
            this.tbInfo.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.tbInfo.ForeColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.tbInfo.Location = new System.Drawing.Point(0, 265);
            this.tbInfo.Multiline = true;
            this.tbInfo.Name = "tbInfo";
            this.tbInfo.ReadOnly = true;
            this.tbInfo.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.tbInfo.Size = new System.Drawing.Size(600, 52);
            this.tbInfo.TabIndex = 2;
            // 
            // pic
            // 
            this.pic.Location = new System.Drawing.Point(0, 0);
            this.pic.Name = "pic";
            this.pic.Size = new System.Drawing.Size(113, 104);
            this.pic.SizeMode = System.Windows.Forms.PictureBoxSizeMode.AutoSize;
            this.pic.TabIndex = 1;
            this.pic.TabStop = false;
            // 
            // menuStrip1
            // 
            this.menuStrip1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolsToolStripMenuItem});
            this.menuStrip1.Location = new System.Drawing.Point(0, 426);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(600, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // toolsToolStripMenuItem
            // 
            this.toolsToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuColorConvertor});
            this.toolsToolStripMenuItem.Name = "toolsToolStripMenuItem";
            this.toolsToolStripMenuItem.Size = new System.Drawing.Size(48, 20);
            this.toolsToolStripMenuItem.Text = "Tools";
            // 
            // mnuColorConvertor
            // 
            this.mnuColorConvertor.Name = "mnuColorConvertor";
            this.mnuColorConvertor.Size = new System.Drawing.Size(180, 22);
            this.mnuColorConvertor.Text = "Color convertor";
            this.mnuColorConvertor.Click += new System.EventHandler(this.mnuColorConvertor_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.menuStrip1);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.panel1);
            this.MainMenuStrip = this.menuStrip1;
            this.Name = "Form1";
            this.Text = "Form1";
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.panel6.ResumeLayout(false);
            this.panel6.PerformLayout();
            this.panel5.ResumeLayout(false);
            this.panel5.PerformLayout();
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pic)).EndInit();
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button btnLoad;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Panel panel6;
        private System.Windows.Forms.RadioButton rbBottomToTop;
        private System.Windows.Forms.RadioButton rbTopToBottom;
        private System.Windows.Forms.Panel panel5;
        private System.Windows.Forms.RadioButton rbRightToLeft;
        private System.Windows.Forms.RadioButton rbLeftToRight;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.RadioButton rb16ColorDots;
        private System.Windows.Forms.RadioButton rbFullColor;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.RadioButton rbTextData;
        private System.Windows.Forms.RadioButton rbBinaryData;
        private System.Windows.Forms.Button btnSaveImage;
        private System.Windows.Forms.RadioButton rbBlackWhiteDots;
        private System.Windows.Forms.CheckBox cbAddToExisted;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.PictureBox pic;
        private System.Windows.Forms.TextBox tbInfo;
        private System.Windows.Forms.RadioButton rb256ColorDots;
        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem toolsToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem mnuColorConvertor;
    }
}

