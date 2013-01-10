using System;
using System.Collections.Generic;
using System.Text;

namespace AbbyPatchBuilder
{
    public class ITEM
    {
        private string archivo;
        private string crc32;

        public ITEM(string a, string c)
        {
            archivo = a;
            crc32 = c;
        }

        public string Crc32
        {
            get { return crc32; }
            set { crc32 = value; }
        }

        public string Archivo
        {
            get { return archivo; }
            set { archivo = value; }
        }

    }
}
