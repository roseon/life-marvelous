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
            this.barraprogreso = new System.Windows.Forms.ProgressBar();
            this.status = new System.Windows.Forms.Label();
            this.PLAY = new System.Windows.Forms.PictureBox();
            this.cerrar = new System.Windows.Forms.PictureBox();
            ((System.ComponentModel.ISupportInitialize)(this.PLAY)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.cerrar)).BeginInit();
            this.SuspendLayout();
            // 
            // barraprogreso
            // 
            this.barraprogreso.Location = new System.Drawing.Point(80, 180);
            this.barraprogreso.Name = "barraprogreso";
            this.barraprogreso.Size = new System.Drawing.Size(243, 23);
            this.barraprogreso.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.barraprogreso.TabIndex = 0;
            this.barraprogreso.Visible = false;
            // 
            // status
            // 
            this.status.AutoSize = true;
            this.status.BackColor = System.Drawing.Color.Transparent;
            this.status.ForeColor = System.Drawing.SystemColors.ControlLight;
            this.status.Location = new System.Drawing.Point(77, 164);
            this.status.Name = "status";
            this.status.Size = new System.Drawing.Size(43, 13);
            this.status.TabIndex = 1;
            this.status.Text = "Status.-";
            // 
            // PLAY
            // 
            this.PLAY.Image = global::AbbyLauncher.Properties.Resources.boton_dis;
            this.PLAY.Location = new System.Drawing.Point(295, 225);
            this.PLAY.Name = "PLAY";
            this.PLAY.Size = new System.Drawing.Size(145, 55);
            this.PLAY.TabIndex = 2;
            this.PLAY.TabStop = false;
            this.PLAY.Click += new System.EventHandler(this.PLAY_Click);
            this.PLAY.MouseLeave += new System.EventHandler(this.PLAY_MouseLeave);
            this.PLAY.MouseHover += new System.EventHandler(this.PLAY_MouseHover);
            // 
            // cerrar
            // 
            this.cerrar.Image = global::AbbyLauncher.Properties.Resources.close;
            this.cerrar.Location = new System.Drawing.Point(427, 4);
            this.cerrar.Name = "cerrar";
            this.cerrar.Size = new System.Drawing.Size(30, 30);
            this.cerrar.TabIndex = 3;
            this.cerrar.TabStop = false;
            this.cerrar.Click += new System.EventHandler(this.cerrar_Click);
            // 
            // AbbyGunz
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackgroundImage = global::AbbyLauncher.Properties.Resources.Launcher;
            this.ClientSize = new System.Drawing.Size(462, 301);
            this.Controls.Add(this.cerrar);
            this.Controls.Add(this.PLAY);
            this.Controls.Add(this.status);
            this.Controls.Add(this.barraprogreso);
            this.DoubleBuffered = true;
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "AbbyGunz";
            this.ShowIcon = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "AbbyGunZ";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.AbbyGunz_MouseDown);
            ((System.ComponentModel.ISupportInitialize)(this.PLAY)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.cerrar)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ProgressBar barraprogreso;
        private System.Windows.Forms.Label status;
        private System.Windows.Forms.PictureBox PLAY;
        private System.Windows.Forms.PictureBox cerrar;
    }
}

