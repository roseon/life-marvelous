using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.IO;

namespace AbbyPatchBuilder
{
    public partial class AbbyPatchBuilder : Form
    {
        private List<ITEM> items;
        private Thread thr;

        public AbbyPatchBuilder()
        {
            InitializeComponent();
        }

        private void AbbyPatchBuilder_Load(object sender, EventArgs e)
        {
            Application.DoEvents();
            CheckForIllegalCrossThreadCalls = false;
            thr = new Thread(listar);
            thr.Start();
        }

        private void listar()
        {
            items = new List<ITEM>();

            string actual = Directory.GetCurrentDirectory();
            string [] dirs = Directory.GetDirectories( actual );
            string c;
            LISTA.Items.Clear();
            foreach (string dir in dirs)
            {
                string [] files = Directory.GetFiles(dir);
                foreach (string file in files)
                {
                    c = file.Replace(actual, "");
                    c = c.Replace("\\", "/");
                    items.Add( new ITEM("." + c, getCRC32(actual + c)));
                    LISTA.Items.Add("." + c);
                    Application.DoEvents();
                }
            }
            string[] filess = Directory.GetFiles(actual);
            foreach (string file in filess)
            {
                c = file.Replace(actual, "");
                c = c.Replace("\\", "/");
                try
                {
                    if (!c.Substring(1).ToLower().Equals(Path.GetFileName(Application.ExecutablePath).ToLower()) && !c.Substring(1).ToLower().Equals("patch.xml"))
                    {
                        items.Add(new ITEM("." + c, getCRC32(actual + c)));
                        LISTA.Items.Add("." + c);
                        Application.DoEvents();
                    }
                }catch(Exception e){}
            }

            
            if(!crearPatch.Enabled)
                crearPatch.Enabled = true;
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

        private void crearPatch_Click(object sender, EventArgs e)
        {
            listar();

            string xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" + Environment.NewLine + "<FILES>" + Environment.NewLine;
            foreach (ITEM i in items)
            {
                xml += "<ITEM nombre=\"" + i.Archivo + "\">" + i.Crc32 + "</ITEM>" + Environment.NewLine;
            }
            xml += "</FILES>";
            File.WriteAllText("Patch.xml", xml);
            MessageBox.Show("Patch.xml Creado Satisfactoriamente!");
        }
    }
}
