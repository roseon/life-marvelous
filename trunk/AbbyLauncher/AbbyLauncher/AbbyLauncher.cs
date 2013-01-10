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
            File.WriteAllBytes("abbyl.dll", serial);
            Thread.Sleep(100);
            Application.DoEvents();
            CrearSerial();
        }
        #endregion

        #region MoverForm
        public const int WM_NCLBUTTONDOWN = 0xA1;
        public const int HT_CAPTION = 0x2;

        [DllImportAttribute("user32.dll")]
        public static extern int SendMessage(IntPtr hWnd,
                         int Msg, int wParam, int lParam);
        [DllImportAttribute("user32.dll")]
        public static extern bool ReleaseCapture();
        private void AbbyGunz_MouseDown(object sender, MouseEventArgs e)
        {
            if (e.Button == MouseButtons.Left)
            {
                ReleaseCapture();
                SendMessage(Handle, WM_NCLBUTTONDOWN, HT_CAPTION, 0);
            }
        }
        #endregion

        public AbbyGunz()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
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
            barraprogreso.Visible = (false);
            if (updateLauncher)
            {
                ActualizarLauncher();
            }
            Lanzar = true;
            Status("Listo!");
            if (Lanzar)
                PLAY.Image = Properties.Resources.boton_on;
        }

        private void ActualizarLauncher()
        {
            string cmd = "taskkill /f /im " + Path.GetFileName(Application.ExecutablePath) + Environment.NewLine +
                "ping -n 2 127.0.0.1 > null" + Environment.NewLine +
                "del /f /q \"" + System.Reflection.Assembly.GetExecutingAssembly().Location + "\" " + Environment.NewLine + 
                "ren " + Path.GetFileName(Application.ExecutablePath) + "_ " + Path.GetFileName(Application.ExecutablePath) + Environment.NewLine +
                "ping -n 1 127.0.0.1 > null" + Environment.NewLine +
                "del /f /q UPDATE.bat" + Environment.NewLine + 
                "start \"" + Directory.GetCurrentDirectory() + "\" " + Path.GetFileName(Application.ExecutablePath) + Environment.NewLine +
                "pause";
            File.WriteAllText("UPDATE.bat", cmd);
            if (thr != null)
                if (thr.IsAlive)
                    thr.Abort();
            Thread.Sleep(2000);
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
            CRC32 crc32 = new CRC32();
            String hash = String.Empty;

            using (FileStream fs = File.Open(archivo, FileMode.Open))
                foreach (byte b in crc32.ComputeHash(fs))
                    hash += b.ToString("x2").ToUpper();

            return hash;
        }

        #region Efecto Y Funciones Botones
        private void PLAY_Click(object sender, EventArgs e)
        {
            if (!Lanzar)
                return;
            PLAY.Image = Properties.Resources.boton_on;
            Application.DoEvents();
            Thread.Sleep(80);
            PLAY.Image = Properties.Resources.boton_off;
            Application.DoEvents();

            System.Diagnostics.Process.Start(Directory.GetCurrentDirectory() + "\\GunZ.exe");
            SerialKey();
            Thread.Sleep(100);
            Application.Exit();
        }

        private void PLAY_MouseHover(object sender, EventArgs e)
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

        private void cerrar_Click(object sender, EventArgs e)
        {
            if (thr != null)
                if (thr.IsAlive)
                    thr.Abort();
            Thread.Sleep(2000);
            Application.Exit();
        }
        #endregion
        

    }
}
