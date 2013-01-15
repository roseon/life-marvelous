namespace AbbyLauncher
{
    partial class AbbyGunz
    {
        /// <summary>
        /// Variable del diseñador requerida.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Limpiar los recursos que se estén utilizando.
        /// </summary>
        /// <param name="disposing">true si los recursos administrados se deben eliminar; false en caso contrario.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Código generado por el Diseñador de Windows Forms

        /// <summary>
        /// Método necesario para admitir el Diseñador. No se puede modificar
        /// el contenido del método con el editor de código.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AbbyGunz));
            this.barraprogreso = new System.Windows.Forms.ProgressBar();
            this.status = new System.Windows.Forms.Label();
            this.PLAY = new System.Windows.Forms.PictureBox();
            this.cerrar = new System.Windows.Forms.PictureBox();
            this.linkLabel1 = new System.Windows.Forms.LinkLabel();
            this.linkLabel2 = new System.Windows.Forms.LinkLabel();
            this.linkLabel3 = new System.Windows.Forms.LinkLabel();
            this.Dragg = new System.Windows.Forms.PictureBox();
            this.min = new System.Windows.Forms.PictureBox();
            this.info = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.PLAY)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.cerrar)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.Dragg)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.min)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.info)).BeginInit();
            this.SuspendLayout();
            // 
            // barraprogreso
            // 
            this.barraprogreso.BackColor = System.Drawing.Color.Black;
            this.barraprogreso.ForeColor = System.Drawing.Color.Goldenrod;
            this.barraprogreso.Location = new System.Drawing.Point(115, 254);
            this.barraprogreso.Name = "barraprogreso";
            this.barraprogreso.Size = new System.Drawing.Size(287, 10);
            this.barraprogreso.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.barraprogreso.TabIndex = 0;
            // 
            // status
            // 
            this.status.AutoSize = true;
            this.status.BackColor = System.Drawing.Color.Transparent;
            this.status.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.status.Location = new System.Drawing.Point(149, 237);
            this.status.Name = "status";
            this.status.Size = new System.Drawing.Size(43, 13);
            this.status.TabIndex = 1;
            this.status.Text = "Status.-";
            // 
            // PLAY
            // 
            this.PLAY.BackColor = System.Drawing.Color.Transparent;
            this.PLAY.Enabled = false;
            this.PLAY.Image = global::AbbyLauncher.Properties.Resources.pause;
            this.PLAY.Location = new System.Drawing.Point(82, 224);
            this.PLAY.Name = "PLAY";
            this.PLAY.Size = new System.Drawing.Size(28, 26);
            this.PLAY.TabIndex = 2;
            this.PLAY.TabStop = false;
            this.PLAY.Click += new System.EventHandler(this.PLAY_Click);
            this.PLAY.MouseDown += new System.Windows.Forms.MouseEventHandler(this.PLAY_MouseDown);
            this.PLAY.MouseLeave += new System.EventHandler(this.PLAY_MouseLeave);
            this.PLAY.MouseMove += new System.Windows.Forms.MouseEventHandler(this.PLAY_MouseMove);
            this.PLAY.MouseUp += new System.Windows.Forms.MouseEventHandler(this.PLAY_MouseUp);
            // 
            // cerrar
            // 
            this.cerrar.BackColor = System.Drawing.Color.Transparent;
            this.cerrar.Image = global::AbbyLauncher.Properties.Resources.close;
            this.cerrar.Location = new System.Drawing.Point(446, 109);
            this.cerrar.Name = "cerrar";
            this.cerrar.Size = new System.Drawing.Size(21, 14);
            this.cerrar.TabIndex = 3;
            this.cerrar.TabStop = false;
            this.cerrar.Click += new System.EventHandler(this.cerrar_Click);
            this.cerrar.MouseDown += new System.Windows.Forms.MouseEventHandler(this.cerrar_MouseDown);
            this.cerrar.MouseLeave += new System.EventHandler(this.cerrar_MouseLeave);
            this.cerrar.MouseMove += new System.Windows.Forms.MouseEventHandler(this.cerrar_MouseMove);
            this.cerrar.MouseUp += new System.Windows.Forms.MouseEventHandler(this.cerrar_MouseUp);
            // 
            // linkLabel1
            // 
            this.linkLabel1.AutoSize = true;
            this.linkLabel1.BackColor = System.Drawing.Color.Transparent;
            this.linkLabel1.Font = new System.Drawing.Font("Arial Narrow", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.linkLabel1.LinkColor = System.Drawing.Color.White;
            this.linkLabel1.Location = new System.Drawing.Point(132, 299);
            this.linkLabel1.Name = "linkLabel1";
            this.linkLabel1.Size = new System.Drawing.Size(37, 20);
            this.linkLabel1.TabIndex = 4;
            this.linkLabel1.TabStop = true;
            this.linkLabel1.Text = "Web";
            this.linkLabel1.VisitedLinkColor = System.Drawing.Color.White;
            this.linkLabel1.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabel1_LinkClicked);
            // 
            // linkLabel2
            // 
            this.linkLabel2.AutoSize = true;
            this.linkLabel2.BackColor = System.Drawing.Color.Transparent;
            this.linkLabel2.Font = new System.Drawing.Font("Arial Narrow", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.linkLabel2.LinkColor = System.Drawing.Color.White;
            this.linkLabel2.Location = new System.Drawing.Point(214, 299);
            this.linkLabel2.Name = "linkLabel2";
            this.linkLabel2.Size = new System.Drawing.Size(44, 20);
            this.linkLabel2.TabIndex = 5;
            this.linkLabel2.TabStop = true;
            this.linkLabel2.Text = "Foros";
            this.linkLabel2.VisitedLinkColor = System.Drawing.Color.White;
            this.linkLabel2.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabel2_LinkClicked);
            // 
            // linkLabel3
            // 
            this.linkLabel3.AutoSize = true;
            this.linkLabel3.BackColor = System.Drawing.Color.Transparent;
            this.linkLabel3.Font = new System.Drawing.Font("Arial Narrow", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.linkLabel3.LinkColor = System.Drawing.Color.White;
            this.linkLabel3.Location = new System.Drawing.Point(293, 299);
            this.linkLabel3.Name = "linkLabel3";
            this.linkLabel3.Size = new System.Drawing.Size(70, 20);
            this.linkLabel3.TabIndex = 6;
            this.linkLabel3.TabStop = true;
            this.linkLabel3.Text = "Facebook";
            this.linkLabel3.VisitedLinkColor = System.Drawing.Color.White;
            this.linkLabel3.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabel3_LinkClicked);
            // 
            // Dragg
            // 
            this.Dragg.BackColor = System.Drawing.Color.Transparent;
            this.Dragg.Location = new System.Drawing.Point(92, 133);
            this.Dragg.Name = "Dragg";
            this.Dragg.Size = new System.Drawing.Size(414, 186);
            this.Dragg.TabIndex = 7;
            this.Dragg.TabStop = false;
            this.Dragg.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Dragg_MouseDown);
            this.Dragg.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Dragg_MouseMove);
            this.Dragg.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Dragg_MouseUp);
            // 
            // min
            // 
            this.min.BackColor = System.Drawing.Color.Transparent;
            this.min.Image = global::AbbyLauncher.Properties.Resources.minim;
            this.min.Location = new System.Drawing.Point(410, 109);
            this.min.Name = "min";
            this.min.Size = new System.Drawing.Size(21, 14);
            this.min.TabIndex = 8;
            this.min.TabStop = false;
            this.min.Click += new System.EventHandler(this.min_Click);
            this.min.MouseDown += new System.Windows.Forms.MouseEventHandler(this.min_MouseDown);
            this.min.MouseLeave += new System.EventHandler(this.min_MouseLeave);
            this.min.MouseMove += new System.Windows.Forms.MouseEventHandler(this.min_MouseMove);
            this.min.MouseUp += new System.Windows.Forms.MouseEventHandler(this.min_MouseUp);
            // 
            // info
            // 
            this.info.BackColor = System.Drawing.Color.Transparent;
            this.info.Image = global::AbbyLauncher.Properties.Resources.info;
            this.info.Location = new System.Drawing.Point(374, 109);
            this.info.Name = "info";
            this.info.Size = new System.Drawing.Size(21, 14);
            this.info.TabIndex = 9;
            this.info.TabStop = false;
            this.info.Click += new System.EventHandler(this.info_Click);
            this.info.MouseDown += new System.Windows.Forms.MouseEventHandler(this.info_MouseDown);
            this.info.MouseLeave += new System.EventHandler(this.info_MouseLeave);
            this.info.MouseMove += new System.Windows.Forms.MouseEventHandler(this.info_MouseMove);
            this.info.MouseUp += new System.Windows.Forms.MouseEventHandler(this.info_MouseUp);
            // 
            // AbbyGunz
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.SystemColors.Control;
            this.BackgroundImage = global::AbbyLauncher.Properties.Resources.launcha3;
            this.ClientSize = new System.Drawing.Size(674, 376);
            this.Controls.Add(this.barraprogreso);
            this.Controls.Add(this.info);
            this.Controls.Add(this.min);
            this.Controls.Add(this.linkLabel3);
            this.Controls.Add(this.linkLabel2);
            this.Controls.Add(this.linkLabel1);
            this.Controls.Add(this.cerrar);
            this.Controls.Add(this.PLAY);
            this.Controls.Add(this.status);
            this.Controls.Add(this.Dragg);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "AbbyGunz";
            this.ShowIcon = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "AbbyGunZ";
            this.TransparencyKey = System.Drawing.SystemColors.Control;
            this.Load += new System.EventHandler(this.Form1_Load);
            ((System.ComponentModel.ISupportInitialize)(this.PLAY)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.cerrar)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.Dragg)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.min)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.info)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ProgressBar barraprogreso;
        private System.Windows.Forms.Label status;
        private System.Windows.Forms.PictureBox PLAY;
        private System.Windows.Forms.PictureBox cerrar;
        private System.Windows.Forms.LinkLabel linkLabel1;
        private System.Windows.Forms.LinkLabel linkLabel2;
        private System.Windows.Forms.LinkLabel linkLabel3;
        private System.Windows.Forms.PictureBox Dragg;
        private System.Windows.Forms.PictureBox min;
        private System.Windows.Forms.PictureBox info;
    }
}

