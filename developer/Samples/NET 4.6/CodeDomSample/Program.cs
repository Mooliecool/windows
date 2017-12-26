using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.CodeDom;
using System.CodeDom.Compiler;

namespace System.Extentions.Cons
{
    class Program
    {
        [STAThread]
        static void Main(string[] args)
        {
            try
            {
                // CodeDom demos
                SimpleDemo.CreateUser();
                SimpleDemo.CreateStronglyTypedCollection();
                SimpleDemo.CreateStronglyTypedDataReader();
                SimpleDemo.CreateStronglyTypedDictionary();               
            }
            catch (Exception ex)
            {
                Console.Write(ex.ToString());
            }
        }
    }
}
