namespace TextCounter
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
            this.btnOpenTxtFile = new System.Windows.Forms.Button();
            this.lbl1 = new System.Windows.Forms.Label();
            this.lbl2 = new System.Windows.Forms.Label();
            this.lb = new System.Windows.Forms.ListBox();
            this.lbl3 = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // btnOpenTxtFile
            // 
            this.btnOpenTxtFile.Location = new System.Drawing.Point(13, 13);
            this.btnOpenTxtFile.Name = "btnOpenTxtFile";
            this.btnOpenTxtFile.Size = new System.Drawing.Size(113, 23);
            this.btnOpenTxtFile.TabIndex = 0;
            this.btnOpenTxtFile.Text = "Open TXT File";
            this.btnOpenTxtFile.UseVisualStyleBackColor = true;
            this.btnOpenTxtFile.Click += new System.EventHandler(this.btnOpenTxtFile_Click);
            // 
            // lbl1
            // 
            this.lbl1.AutoSize = true;
            this.lbl1.Location = new System.Drawing.Point(13, 55);
            this.lbl1.Name = "lbl1";
            this.lbl1.Size = new System.Drawing.Size(66, 13);
            this.lbl1.TabIndex = 1;
            this.lbl1.Text = "Букв всего:";
            // 
            // lbl2
            // 
            this.lbl2.AutoSize = true;
            this.lbl2.Location = new System.Drawing.Point(13, 78);
            this.lbl2.Name = "lbl2";
            this.lbl2.Size = new System.Drawing.Size(107, 13);
            this.lbl2.TabIndex = 2;
            this.lbl2.Text = "Из них уникальных:";
            // 
            // lb
            // 
            this.lb.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(204)));
            this.lb.FormattingEnabled = true;
            this.lb.ItemHeight = 14;
            this.lb.Location = new System.Drawing.Point(12, 119);
            this.lb.Name = "lb";
            this.lb.ScrollAlwaysVisible = true;
            this.lb.Size = new System.Drawing.Size(267, 368);
            this.lb.TabIndex = 3;
            // 
            // lbl3
            // 
            this.lbl3.AutoSize = true;
            this.lbl3.Location = new System.Drawing.Point(13, 102);
            this.lbl3.Name = "lbl3";
            this.lbl3.Size = new System.Drawing.Size(125, 13);
            this.lbl3.TabIndex = 4;
            this.lbl3.Text = "Не использовано букв:";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(329, 499);
            this.Controls.Add(this.lbl3);
            this.Controls.Add(this.lb);
            this.Controls.Add(this.lbl2);
            this.Controls.Add(this.lbl1);
            this.Controls.Add(this.btnOpenTxtFile);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnOpenTxtFile;
        private System.Windows.Forms.Label lbl1;
        private System.Windows.Forms.Label lbl2;
        private System.Windows.Forms.ListBox lb;
        private System.Windows.Forms.Label lbl3;
    }
}

