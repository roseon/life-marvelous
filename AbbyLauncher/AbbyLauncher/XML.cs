using System;
using System.Collections.Generic;
using System.Xml;

namespace AbbyLauncher
{
    public class XML
    {
        private List<ITEM> lista = new List<ITEM>();
        public List<ITEM> getArchivos(string archivo)
        {
            ITEM item;

            XmlDocument xDoc = new XmlDocument();
            xDoc.Load(archivo);

            XmlNodeList FILES = xDoc.SelectNodes("//ITEM");
            foreach (XmlNode nodos in FILES)
            {
                XmlElement nodo = (XmlElement)nodos;
                item = new ITEM( nodo.GetAttribute("nombre"), (nodos.InnerText).ToUpper() );

                lista.Add(item);
            }

            return lista;
        }

        
    }
}
