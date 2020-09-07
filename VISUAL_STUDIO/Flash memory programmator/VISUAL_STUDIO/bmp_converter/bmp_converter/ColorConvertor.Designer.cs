namespace bmp_converter
{
    partial class frmColorConvertor
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
            this.label1 = new System.Windows.Forms.Label();
            this.tbWR = new System.Windows.Forms.TextBox();
            this.tbWG = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.tbWB = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.tbIB = new System.Windows.Forms.TextBox();
            this.tbIG = new System.Windows.Forms.TextBox();
            this.tbIR = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.btnToILI = new System.Windows.Forms.Button();
            this.btnToWin = new System.Windows.Forms.Button();
            this.tbWRGB = new System.Windows.Forms.TextBox();
            this.tbIRGB = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.pnlColor = new System.Windows.Forms.Panel();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(107, 45);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(15, 13);
            this.label1.TabIndex = 0;
            this.label1.Text = "R";
            // 
            // tbWR
            // 
            this.tbWR.Location = new System.Drawing.Point(33, 38);
            this.tbWR.Name = "tbWR";
            this.tbWR.ReadOnly = true;
            this.tbWR.Size = new System.Drawing.Size(51, 20);
            this.tbWR.TabIndex = 1;
            // 
            // tbWG
            // 
            this.tbWG.Location = new System.Drawing.Point(33, 64);
            this.tbWG.Name = "tbWG";
            this.tbWG.ReadOnly = true;
            this.tbWG.Size = new System.Drawing.Size(51, 20);
            this.tbWG.TabIndex = 3;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(107, 71);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(15, 13);
            this.label2.TabIndex = 2;
            this.label2.Text = "G";
            // 
            // tbWB
            // 
            this.tbWB.Location = new System.Drawing.Point(33, 90);
            this.tbWB.Name = "tbWB";
            this.tbWB.ReadOnly = true;
            this.tbWB.Size = new System.Drawing.Size(51, 20);
            this.tbWB.TabIndex = 5;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(107, 97);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(14, 13);
            this.label3.TabIndex = 4;
            this.label3.Text = "B";
            // 
            // tbIB
            // 
            this.tbIB.Location = new System.Drawing.Point(142, 90);
            this.tbIB.Name = "tbIB";
            this.tbIB.ReadOnly = true;
            this.tbIB.Size = new System.Drawing.Size(51, 20);
            this.tbIB.TabIndex = 8;
            // 
            // tbIG
            // 
            this.tbIG.Location = new System.Drawing.Point(142, 64);
            this.tbIG.Name = "tbIG";
            this.tbIG.ReadOnly = true;
            this.tbIG.Size = new System.Drawing.Size(51, 20);
            this.tbIG.TabIndex = 7;
            // 
            // tbIR
            // 
            this.tbIR.Location = new System.Drawing.Point(142, 38);
            this.tbIR.Name = "tbIR";
            this.tbIR.ReadOnly = true;
            this.tbIR.Size = new System.Drawing.Size(51, 20);
            this.tbIR.TabIndex = 6;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(34, 9);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(51, 13);
            this.label4.TabIndex = 9;
            this.label4.Text = "Windows";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(148, 9);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(43, 13);
            this.label5.TabIndex = 10;
            this.label5.Text = "ILI9341";
            // 
            // btnToILI
            // 
            this.btnToILI.Location = new System.Drawing.Point(35, 183);
            this.btnToILI.Name = "btnToILI";
            this.btnToILI.Size = new System.Drawing.Size(154, 23);
            this.btnToILI.TabIndex = 11;
            this.btnToILI.Text = ">>>";
            this.btnToILI.UseVisualStyleBackColor = true;
            this.btnToILI.Click += new System.EventHandler(this.btnToILI_Click);
            // 
            // btnToWin
            // 
            this.btnToWin.Location = new System.Drawing.Point(35, 212);
            this.btnToWin.Name = "btnToWin";
            this.btnToWin.Size = new System.Drawing.Size(154, 23);
            this.btnToWin.TabIndex = 12;
            this.btnToWin.Text = "<<<";
            this.btnToWin.UseVisualStyleBackColor = true;
            this.btnToWin.Click += new System.EventHandler(this.btnToWin_Click);
            // 
            // tbWRGB
            // 
            this.tbWRGB.Location = new System.Drawing.Point(26, 116);
            this.tbWRGB.Name = "tbWRGB";
            this.tbWRGB.Size = new System.Drawing.Size(69, 20);
            this.tbWRGB.TabIndex = 13;
            // 
            // tbIRGB
            // 
            this.tbIRGB.Location = new System.Drawing.Point(134, 116);
            this.tbIRGB.Name = "tbIRGB";
            this.tbIRGB.Size = new System.Drawing.Size(69, 20);
            this.tbIRGB.TabIndex = 14;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(11, 123);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(18, 13);
            this.label6.TabIndex = 15;
            this.label6.Text = "0x";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(119, 123);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(18, 13);
            this.label7.TabIndex = 16;
            this.label7.Text = "0x";
            // 
            // pnlColor
            // 
            this.pnlColor.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pnlColor.Location = new System.Drawing.Point(37, 143);
            this.pnlColor.Name = "pnlColor";
            this.pnlColor.Size = new System.Drawing.Size(154, 26);
            this.pnlColor.TabIndex = 17;
            // 
            // frmColorConvertor
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(220, 247);
            this.Controls.Add(this.pnlColor);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.tbIRGB);
            this.Controls.Add(this.tbWRGB);
            this.Controls.Add(this.btnToWin);
            this.Controls.Add(this.btnToILI);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.tbIB);
            this.Controls.Add(this.tbIG);
            this.Controls.Add(this.tbIR);
            this.Controls.Add(this.tbWB);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.tbWG);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.tbWR);
            this.Controls.Add(this.label1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.MaximizeBox = false;
            this.Name = "frmColorConvertor";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "ColorConvertor";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox tbWR;
        private System.Windows.Forms.TextBox tbWG;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox tbWB;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox tbIB;
        private System.Windows.Forms.TextBox tbIG;
        private System.Windows.Forms.TextBox tbIR;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Button btnToILI;
        private System.Windows.Forms.Button btnToWin;
        private System.Windows.Forms.TextBox tbWRGB;
        private System.Windows.Forms.TextBox tbIRGB;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Panel pnlColor;
    }
}