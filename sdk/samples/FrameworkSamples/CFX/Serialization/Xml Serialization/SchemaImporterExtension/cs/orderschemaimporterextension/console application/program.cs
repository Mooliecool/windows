#region Using directives

using System;
using System.Collections;
using System.IO;
using System.Collections.Generic;
using System.Text;
using System.Xml.Serialization;
using System.Xml;
using System.Xml.Schema;
using System.Web.Services.Description;
using System.Web.Services.Discovery;
using System.CodeDom;
using System.CodeDom.Compiler;
using Microsoft.CSharp;

#endregion

namespace Microsoft.Samples
{
	class Program
	{
		static void Main(string[] args)
		{

			Uri wsdlLocation = new Uri(Directory.GetCurrentDirectory() + "\\..\\..\\..\\OrderService.wsdl");

			Console.WriteLine("Beginning schema import.");
			Console.WriteLine();

            DiscoveryClientProtocol client = new DiscoveryClientProtocol();
            client.AllowAutoRedirect = true;
            DiscoveryDocument doc = client.DiscoverAny (wsdlLocation.ToString());
            client.ResolveAll();
            
            WebReferenceCollection references=new WebReferenceCollection();
            
            //add 1st web reference document
			WebReference wr = new WebReference(client.Documents, new CodeNamespace("Microsoft.Samples"));
			references.Add(wr);
            
            //add other web reference documents if needed
            
            CodeDomProvider codeProvider = new    
                          Microsoft.CSharp.CSharpCodeProvider ();
            CodeCompileUnit Ccu=new CodeCompileUnit();
            Ccu.Namespaces.Add(wr.ProxyCode);
            try
            {
                   //This is the new API call
                   ServiceDescriptionImporter.GenerateWebReferences(
					   references, 
					   codeProvider, Ccu,new WebReferenceOptions());
            }
            catch(Exception ex)
            {
                //do exception handling
                Console.WriteLine("An exception occured during importation:\n {0}", ex.ToString());
            }
            
            StreamWriter writer = new StreamWriter ("OrderProxy.cs", false, 
                                     new System.Text.UTF8Encoding (true));

            codeProvider.GenerateCodeFromCompileUnit(Ccu, writer, new CodeGeneratorOptions());
             writer.Close ();   


//			importer.Import(new CodeNamespace("Microsoft.Samples"), compileUnit);
			Console.WriteLine();
			Console.WriteLine("Schema import completed.");

			//reader.Close();

			Console.WriteLine("Finished. Press any key to continue...");
			Console.ReadLine();

			System.Diagnostics.Process.Start("notepad", "OrderProxy.cs");

		}
	}
}
