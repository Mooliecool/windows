using System;
using System.IO;
using System.Collections.Generic;
using System.Xml.Linq;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RDFSample
{
    class Program
    {
        static void Main(string[] args)
        {
            string path = Directory.GetCurrentDirectory(); 
            XDocument xdoc = new XDocument(new XElement("Root", DirToXml(new DirectoryInfo(path))));
            xdoc.Save("Root.xml", SaveOptions.None);
        }

        private static XElement DirToXml(DirectoryInfo dir)
        {
            return new XElement("Directory",
                        new XAttribute("Name", dir.Name),
                        dir.GetDirectories().Select(d => DirToXml(d)));
        }
    }
}
