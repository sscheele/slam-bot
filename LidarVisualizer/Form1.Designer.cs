
namespace LidarVisualizer
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.receiveButton = new System.Windows.Forms.Button();
            this.clearButton = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // receiveButton
            // 
            this.receiveButton.Location = new System.Drawing.Point(89, 399);
            this.receiveButton.Name = "receiveButton";
            this.receiveButton.Size = new System.Drawing.Size(105, 50);
            this.receiveButton.TabIndex = 0;
            this.receiveButton.Text = "Receive Data";
            this.receiveButton.UseVisualStyleBackColor = true;
            this.receiveButton.Click += new System.EventHandler(this.receiveButtonClick);
            // 
            // clearButton
            // 
            this.clearButton.ImageAlign = System.Drawing.ContentAlignment.BottomLeft;
            this.clearButton.Location = new System.Drawing.Point(227, 399);
            this.clearButton.Name = "clearButton";
            this.clearButton.Size = new System.Drawing.Size(105, 50);
            this.clearButton.TabIndex = 1;
            this.clearButton.Text = "Clear Map";
            this.clearButton.UseVisualStyleBackColor = true;
            this.clearButton.Click += new System.EventHandler(this.clearButtonClick);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(422, 518);
            this.Controls.Add(this.receiveButton);
            this.Controls.Add(this.clearButton);
            this.Name = "Form1";
            this.Text = "Lidar Visualizer";
            this.Load += new System.EventHandler(this.form1_Load);
            this.Paint += new System.Windows.Forms.PaintEventHandler(this.form1_Draw);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button receiveButton;
        private System.Windows.Forms.Button clearButton;
        

    }
}

