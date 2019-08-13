namespace RemoteControl
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
            this.button_connect = new System.Windows.Forms.Button();
            this.listBox_events = new System.Windows.Forms.ListBox();
            this.button_motor = new System.Windows.Forms.Button();
            this.label_angleVal = new System.Windows.Forms.Label();
            this.trackBar_kp = new System.Windows.Forms.TrackBar();
            this.trackBar_ki = new System.Windows.Forms.TrackBar();
            this.trackBar_kd = new System.Windows.Forms.TrackBar();
            this.label_kp_val = new System.Windows.Forms.Label();
            this.label_ki_val = new System.Windows.Forms.Label();
            this.label_kd_val = new System.Windows.Forms.Label();
            this.trackBar_at = new System.Windows.Forms.TrackBar();
            this.label_at_val = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_kp)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_ki)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_kd)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_at)).BeginInit();
            this.SuspendLayout();
            // 
            // button_connect
            // 
            this.button_connect.Location = new System.Drawing.Point(12, 12);
            this.button_connect.Name = "button_connect";
            this.button_connect.Size = new System.Drawing.Size(75, 23);
            this.button_connect.TabIndex = 0;
            this.button_connect.Text = "Connect";
            this.button_connect.UseVisualStyleBackColor = true;
            this.button_connect.Click += new System.EventHandler(this.button_connect_Click);
            // 
            // listBox_events
            // 
            this.listBox_events.FormattingEnabled = true;
            this.listBox_events.Location = new System.Drawing.Point(594, 12);
            this.listBox_events.Name = "listBox_events";
            this.listBox_events.Size = new System.Drawing.Size(409, 433);
            this.listBox_events.TabIndex = 1;
            // 
            // button_motor
            // 
            this.button_motor.Location = new System.Drawing.Point(48, 80);
            this.button_motor.Name = "button_motor";
            this.button_motor.Size = new System.Drawing.Size(75, 23);
            this.button_motor.TabIndex = 2;
            this.button_motor.Text = "Motor";
            this.button_motor.UseVisualStyleBackColor = true;
            this.button_motor.Click += new System.EventHandler(this.button_motor_Click);
            // 
            // label_angleVal
            // 
            this.label_angleVal.AutoSize = true;
            this.label_angleVal.Location = new System.Drawing.Point(64, 234);
            this.label_angleVal.Name = "label_angleVal";
            this.label_angleVal.Size = new System.Drawing.Size(33, 13);
            this.label_angleVal.TabIndex = 3;
            this.label_angleVal.Text = "angle";
            // 
            // trackBar_kp
            // 
            this.trackBar_kp.Location = new System.Drawing.Point(12, 310);
            this.trackBar_kp.Maximum = 400;
            this.trackBar_kp.Name = "trackBar_kp";
            this.trackBar_kp.Size = new System.Drawing.Size(450, 45);
            this.trackBar_kp.TabIndex = 4;
            this.trackBar_kp.ValueChanged += new System.EventHandler(this.trackBar_kp_ValueChanged);
            // 
            // trackBar_ki
            // 
            this.trackBar_ki.Location = new System.Drawing.Point(12, 361);
            this.trackBar_ki.Maximum = 400;
            this.trackBar_ki.Name = "trackBar_ki";
            this.trackBar_ki.Size = new System.Drawing.Size(450, 45);
            this.trackBar_ki.TabIndex = 4;
            this.trackBar_ki.ValueChanged += new System.EventHandler(this.trackBar_ki_ValueChanged);
            // 
            // trackBar_kd
            // 
            this.trackBar_kd.Location = new System.Drawing.Point(12, 412);
            this.trackBar_kd.Maximum = 400;
            this.trackBar_kd.Name = "trackBar_kd";
            this.trackBar_kd.Size = new System.Drawing.Size(450, 45);
            this.trackBar_kd.TabIndex = 4;
            this.trackBar_kd.ValueChanged += new System.EventHandler(this.trackBar_kd_ValueChanged);
            // 
            // label_kp_val
            // 
            this.label_kp_val.AutoSize = true;
            this.label_kp_val.Location = new System.Drawing.Point(491, 310);
            this.label_kp_val.Name = "label_kp_val";
            this.label_kp_val.Size = new System.Drawing.Size(19, 13);
            this.label_kp_val.TabIndex = 5;
            this.label_kp_val.Text = "kp";
            // 
            // label_ki_val
            // 
            this.label_ki_val.AutoSize = true;
            this.label_ki_val.Location = new System.Drawing.Point(491, 361);
            this.label_ki_val.Name = "label_ki_val";
            this.label_ki_val.Size = new System.Drawing.Size(15, 13);
            this.label_ki_val.TabIndex = 5;
            this.label_ki_val.Text = "ki";
            // 
            // label_kd_val
            // 
            this.label_kd_val.AutoSize = true;
            this.label_kd_val.Location = new System.Drawing.Point(491, 412);
            this.label_kd_val.Name = "label_kd_val";
            this.label_kd_val.Size = new System.Drawing.Size(19, 13);
            this.label_kd_val.TabIndex = 5;
            this.label_kd_val.Text = "kd";
            // 
            // trackBar_at
            // 
            this.trackBar_at.Location = new System.Drawing.Point(12, 532);
            this.trackBar_at.Maximum = 400;
            this.trackBar_at.Name = "trackBar_at";
            this.trackBar_at.Size = new System.Drawing.Size(450, 45);
            this.trackBar_at.TabIndex = 4;
            this.trackBar_at.ValueChanged += new System.EventHandler(this.trackBar_at_ValueChanged);
            // 
            // label_at_val
            // 
            this.label_at_val.AutoSize = true;
            this.label_at_val.Location = new System.Drawing.Point(487, 532);
            this.label_at_val.Name = "label_at_val";
            this.label_at_val.Size = new System.Drawing.Size(33, 13);
            this.label_at_val.TabIndex = 5;
            this.label_at_val.Text = "angle";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1015, 688);
            this.Controls.Add(this.label_at_val);
            this.Controls.Add(this.label_kd_val);
            this.Controls.Add(this.label_ki_val);
            this.Controls.Add(this.label_kp_val);
            this.Controls.Add(this.trackBar_at);
            this.Controls.Add(this.trackBar_kd);
            this.Controls.Add(this.trackBar_ki);
            this.Controls.Add(this.trackBar_kp);
            this.Controls.Add(this.label_angleVal);
            this.Controls.Add(this.button_motor);
            this.Controls.Add(this.listBox_events);
            this.Controls.Add(this.button_connect);
            this.Name = "Form1";
            this.Text = "Remote Control";
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_kp)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_ki)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_kd)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar_at)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button button_connect;
        private System.Windows.Forms.ListBox listBox_events;
        private System.Windows.Forms.Button button_motor;
        private System.Windows.Forms.Label label_angleVal;
        private System.Windows.Forms.TrackBar trackBar_kp;
        private System.Windows.Forms.TrackBar trackBar_ki;
        private System.Windows.Forms.TrackBar trackBar_kd;
        private System.Windows.Forms.Label label_kp_val;
        private System.Windows.Forms.Label label_ki_val;
        private System.Windows.Forms.Label label_kd_val;
        private System.Windows.Forms.TrackBar trackBar_at;
        private System.Windows.Forms.Label label_at_val;
    }
}

