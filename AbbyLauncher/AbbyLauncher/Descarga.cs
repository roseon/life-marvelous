using System;
using System.Net;
using System.Windows.Forms;
using System.IO;

namespace AbbyLauncher
{
    public class Descarga
    {
        private WebClient cliente;
        public int porcenaje;

        public void descargar(string url, string archivo)
        {
            cliente = new WebClient();
            cliente.DownloadProgressChanged += new DownloadProgressChangedEventHandler(cliente_progresschanged);
            cliente.DownloadFileCompleted += new System.ComponentModel.AsyncCompletedEventHandler(cliente_descargacompleta);
            string descarga = url + archivo;
            if (archivo.ToLower().Equals("./" + Path.GetFileName(Application.ExecutablePath).ToLower()))
                archivo += "_";
            cliente.DownloadFileAsync(new Uri(descarga), archivo);
            porcenaje = 0;
        }

        public int getPorcentaje()
        {
            return porcenaje;
        }

        private void cliente_progresschanged(object sender, DownloadProgressChangedEventArgs e)
        {
            double bytesIn = double.Parse(e.BytesReceived.ToString());
            double totalBytes = double.Parse(e.TotalBytesToReceive.ToString());
            double percentage = bytesIn / totalBytes * 100;
            porcenaje = int.Parse(Math.Truncate(percentage).ToString());
        }

        private void cliente_descargacompleta(object sender, System.ComponentModel.AsyncCompletedEventArgs e)
        {
            porcenaje = 100;
        }

    }
}
