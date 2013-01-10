namespace AbbyPatchBuilder
{
    partial class AbbyPatchBuilder
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(AbbyPatchBuilder));
            this.LISTA = new System.Windows.Forms.ListBox();
            this.crearPatch = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // LISTA
            // 
            this.LISTA.FormattingEnabled = true;
            this.LISTA.Location = new System.Drawing.Point(12, 12);
            this.LISTA.Name = "LISTA";
            this.LISTA.Size = new System.Drawing.Size(336, 355);
            this.LISTA.TabIndex = 0;
            // 
            // crearPatch
            // 
            this.crearPatch.Enabled = false;
            this.crearPatch.Location = new System.Drawing.Point(12, 373);
            this.crearPatch.Name = "crearPatch";
            this.crearPatch.Size = new System.Drawing.Size(336, 37);
            this.crearPatch.TabIndex = 1;
            this.crearPatch.Text = "Crear Patch.xml";
            this.crearPatch.UseVisualStyleBackColor = true;
            this.crearPatch.Click += new System.EventHandler(this.crearPatch_Click);
            // 
            // AbbyPatchBuilder
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(360, 422);
            this.Controls.Add(this.crearPatch);
            this.Controls.Add(this.LISTA);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "AbbyPatchBuilder";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "AbbyPatchBuilder";
            this.Load += new System.EventHandler(this.AbbyPatchBuilder_Load);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ListBox LISTA;
        private System.Windows.Forms.Button crearPatch;
    }
}

