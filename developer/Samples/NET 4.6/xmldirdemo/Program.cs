using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Directory;

namespace xmldirdemo
{
    class Program
    {
        static void Main(string[] args)
        {
            string path = Directory.GetCurrentDirectory();                       
            XmlDirectoryLister xmlString = new XmlDirectoryLister(false);
            string compledir = xmlString.GetXmlString(path);
            XmlDocument doc = new XmlDocument(); 
            doc.LoadXml(compledir);
            doc.Save("data.xml");
        }
    }
}
