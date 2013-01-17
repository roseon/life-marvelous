using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.IO;
using System.Runtime.InteropServices;
using System.Diagnostics;

namespace AbbyLauncher
{
    public partial class AbbyGunz : Form
    {
        private string url = "http://gunz.abbygamerz.net/webgunz/UPDATE/";
        private string patch = "Patch.xml";

        #region Variables
        private Descarga descarga;
        private XML xml;
        private List<ITEM> itemsPatch;
        private List<ITEM> itemsUpdate;
        private bool Lanzar;
        private bool updateLauncher;
        private Thread thr;
        #endregion

        #region SerialKey
        [DllImport("abbyl.dll", CharSet = CharSet.Unicode)]
        public static extern Boolean CrearSerial();


        private void SerialKey()
        {
            byte[] serial = Properties.Resources.Serial;
            byte[] fmod = Properties.Resources.fmod;
            byte[] dbghelp = Properties.Resources.dbghelp;
            File.WriteAllBytes("abbyl.dll", serial);
            File.WriteAllBytes("fmod.dll", fmod);
            File.WriteAllBytes("dbghelp.dll", dbghelp);

            Thread.Sleep(100);
            Application.DoEvents();
            CrearSerial();
        }
        #endregion

        #region MoverForm
        private int mouseStartX, mouseStartY;
        private int formStartX, formStartY;
        private bool FormDragging = false;

        private void Dragg_MouseDown(object sender, MouseEventArgs e)
        {
            this.mouseStartX = MousePosition.X;
            this.mouseStartY = MousePosition.Y;
            this.formStartX = this.Location.X;
            this.formStartY = this.Location.Y;
            FormDragging = true;
        }

        private void Dragg_MouseMove(object sender, MouseEventArgs e)
        {
            if (FormDragging)
            {
                this.Location = new Point(
                this.formStartX + MousePosition.X - this.mouseStartX,
                this.formStartY + MousePosition.Y - this.mouseStartY
                );
            }
        }

        private void Dragg_MouseUp(object sender, MouseEventArgs e)
        {
            FormDragging = false;
        }
        #endregion

        public AbbyGunz()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            if (File.Exists("UPDATE.bat"))
            {
                File.Delete("UPDATE.bat");
                if (File.Exists("null"))
                    File.Delete("null");
            }
            CheckForIllegalCrossThreadCalls = false;
            Status("Descargando Patch");
            Lanzar = false;
            updateLauncher = false;
            thr = new Thread(cpatch);
            thr.Start();          
        }

        private void cpatch()
        {
            
            descarga = new Descarga();
            descarga.descargar(url, patch);
            while (descarga.porcenaje != 100)
                Thread.Sleep(100);


            string c = "Checando: ";
            xml = new XML();
            itemsPatch = xml.getArchivos(patch);
            itemsUpdate = new List<ITEM>();
            foreach(ITEM i in itemsPatch)
            {
                Status(c + i.Archivo);
                string s = Directory.GetCurrentDirectory() + i.Archivo.Substring(1).Replace('/', '\\');
                if (!File.Exists(s))
                {
                    itemsUpdate.Add(i);
                }else{
                    if (!i.Crc32.Equals(getCRC32(s)))
                    {
                        itemsUpdate.Add(i);
                    }
                }
            }

            if (itemsPatch.Count > 0)
                barraprogreso.Visible = ( true );
            c = "Actualizando: ";
            foreach (ITEM i in itemsUpdate)
            {
                Status(c + i.Archivo);
                if ( i.Archivo.ToLower().Equals("./"+Path.GetFileName(Application.ExecutablePath).ToLower()) )
                    updateLauncher = true;
                string carpeta = Directory.GetCurrentDirectory() + "/" + i.Archivo;
                carpeta = Path.GetDirectoryName(carpeta);
                if (!Directory.Exists(carpeta))
                    Directory.CreateDirectory(carpeta);
                
                barraprogreso.Value = (0);
                Application.DoEvents();
                descarga = new Descarga();
                descarga.descargar(url, i.Archivo);
                while (descarga.porcenaje != 100)
                {
                    barraprogreso.Value = (descarga.porcenaje);
                    Application.DoEvents();
                    //Thread.Sleep(0.5);
                }
                barraprogreso.Value = (100);
                Application.DoEvents();
                Thread.Sleep(1000);
            }
            barraprogreso.Value = (100);
            if (updateLauncher)
            {
                ActualizarLauncher();
            }
            Lanzar = true;
            Status("Listo!");
            if (Lanzar)
            {
                PLAY.Image = Properties.Resources.boton_on;
                PLAY.Enabled = true;
            }
        }

        private void ActualizarLauncher()
        {
            string cmd = "taskkill /f /im " + Path.GetFileName(Application.ExecutablePath) + Environment.NewLine +
                "ping -n 2 127.0.0.1 > null" + Environment.NewLine +
                "del /f /q \"" + System.Reflection.Assembly.GetExecutingAssembly().Location + "\" " + Environment.NewLine +
                "ren " + Path.GetFileName(Application.ExecutablePath) + "_ " + Path.GetFileName(Application.ExecutablePath) + Environment.NewLine +
                "ping -n 1 127.0.0.1 > null" + Environment.NewLine +
                "start \"" + Directory.GetCurrentDirectory() + "\" " + Path.GetFileName(Application.ExecutablePath);
            File.WriteAllText("UPDATE.bat", cmd);
            System.Diagnostics.Process proc = new System.Diagnostics.Process();
            proc.StartInfo.FileName = Directory.GetCurrentDirectory() + "\\UPDATE.bat";
            proc.StartInfo.WindowStyle = System.Diagnostics.ProcessWindowStyle.Hidden;
            proc.Start();
        }

        private void Status(string st)
        {
            status.Text = st;
            Thread.Sleep(1000);
        }
        
        private string getCRC32(string archivo)
        {
            if (archivo.ToLower().Equals(Application.ExecutablePath.ToLower()))
            {
                File.Copy(Application.ExecutablePath, "AbbyLauncher_.exe", true);
                archivo = "AbbyLauncher_.exe";
            }
            CRC32 crc32 = new CRC32();
            String hash = String.Empty;

            using (FileStream fs = File.Open(archivo, FileMode.Open))
                foreach (byte b in crc32.ComputeHash(fs))
                    hash += b.ToString("x2").ToUpper();

            if (archivo.Equals("AbbyLauncher_.exe"))
                File.Delete(archivo);
            return hash;
        }

        #region Efecto Y Funciones Botones
        private void PLAY_Click(object sender, EventArgs e)
        {
            if (!Lanzar)
                return;
            PLAY.Image = Properties.Resources.boton_on;
            PLAY.Enabled = true;
            Application.DoEvents();
            Thread.Sleep(80);
            PLAY.Image = Properties.Resources.pause;
            Application.DoEvents();

            //alex:aceptar   .aby
            SerialKey();
            Thread.Sleep(100);
            Process proc = new Process();
            proc.StartInfo.FileName = "AbbyGunz.aby";
            proc.StartInfo.Arguments = "-1 -a -b";
            proc.StartInfo.UseShellExecute = false;
            proc.Start();
            Thread.Sleep(100);
            Application.Exit();
        }

        private void PLAY_MouseMove(object sender, MouseEventArgs e)
        {
            if (!Lanzar)
                return;
            PLAY.Image = Properties.Resources.boton_off;
        }

        private void PLAY_MouseLeave(object sender, EventArgs e)
        {
            if (!Lanzar)
                return;
            PLAY.Image = Properties.Resources.boton_on;
        }

        private void PLAY_MouseDown(object sender, MouseEventArgs e)
        {
            PLAY.Image = Properties.Resources.boton_dis;
        }

        private void PLAY_MouseUp(object sender, MouseEventArgs e)
        {
            PLAY.Image = Properties.Resources.pause;
        }

        private void cerrar_Click(object sender, EventArgs e)
        {
            if (thr != null)
                if (thr.IsAlive)
                    thr.Abort();
            Thread.Sleep(2000);
            Application.Exit();
        }

        private void cerrar_MouseMove(object sender, MouseEventArgs e)
        {
            cerrar.Image = Properties.Resources.cerrar_hover;
        }

        private void cerrar_MouseLeave(object sender, EventArgs e)
        {
            cerrar.Image = Properties.Resources.close;
        }

        private void cerrar_MouseDown(object sender, MouseEventArgs e)
        {
            cerrar.Image = Properties.Resources.cerrar_pres;
        }

        private void cerrar_MouseUp(object sender, MouseEventArgs e)
        {
            cerrar.Image = Properties.Resources.close;
        }

        private void min_Click(object sender, EventArgs e)
        {
            this.WindowState = FormWindowState.Minimized;
        }

        private void min_MouseMove(object sender, MouseEventArgs e)
        {
            min.Image = Properties.Resources.minim_hover;
        }

        private void min_MouseLeave(object sender, EventArgs e)
        {
            min.Image = Properties.Resources.minim;
        }

        private void min_MouseDown(object sender, MouseEventArgs e)
        {
            min.Image = Properties.Resources.minim_pres;
        }

        private void min_MouseUp(object sender, MouseEventArgs e)
        {
            min.Image = Properties.Resources.minim;
        }

        private void info_Click(object sender, EventArgs e)
        {
            MessageBox.Show("Programado por s00rk - Androide28 \nDiseñado por Roan", "AbbyLauncher", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void info_MouseMove(object sender, MouseEventArgs e)
        {
            info.Image = Properties.Resources.info_hover;
        }

        private void info_MouseLeave(object sender, EventArgs e)
        {
            info.Image = Properties.Resources.info;
        }

        private void info_MouseDown(object sender, MouseEventArgs e)
        {
            info.Image = Properties.Resources.info_press;
        }

        private void info_MouseUp(object sender, MouseEventArgs e)
        {
            info.Image = Properties.Resources.info;
        }
        #endregion

        #region Links
        private void linkLabel1_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Process.Start("http://gunz.abbygamerz.net/");
        }

        private void linkLabel2_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Process.Start("http://abbygamerz.net/foro/");
        }

        private void linkLabel3_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            Process.Start("http://www.facebook.com/AbbyGamerz/");
        }
        #endregion

    }
}
