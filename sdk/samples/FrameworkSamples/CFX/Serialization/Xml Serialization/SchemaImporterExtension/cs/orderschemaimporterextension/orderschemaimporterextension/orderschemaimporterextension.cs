#region Using directives

using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Text;
using System.Xml.Serialization;
using System.Xml.Serialization.Advanced;
using System.Xml.Schema;
using System.Xml;
using System.CodeDom;
using System.CodeDom.Compiler;
using Microsoft.Samples;

#endregion

namespace Microsoft.Samples
{
	public class OrderSchemaImporterExtension : SchemaImporterExtension
	{
		static OrderSchemaImporterExtension()
		{
		}

		public override string ImportSchemaType(XmlSchemaType type,
			XmlSchemaObject context, XmlSchemas schemas, XmlSchemaImporter importer,
			CodeCompileUnit compileUnit, CodeNamespace codeNamespace, 
            CodeGenerationOptions options, CodeDomProvider codeGenerator)
		{

			ConsoleColor defaultColor = Console.BackgroundColor;
			Console.BackgroundColor = ConsoleColor.Blue;
			Console.WriteLine("\tOrderExtension called for type: " + type.ToString());
			Console.BackgroundColor = defaultColor;

			return null;
		}

	
		public override string ImportSchemaType(string name, string ns, 
			XmlSchemaObject context, XmlSchemas schemas, XmlSchemaImporter importer, 
			CodeCompileUnit compileUnit, CodeNamespace codeNamespace, 
			CodeGenerationOptions options, CodeDomProvider codeGenerator)
		{
			ConsoleColor defaultColor = Console.BackgroundColor;
			Console.BackgroundColor = ConsoleColor.Blue;
			Console.WriteLine("\tOrderExtension called for name = " + name + " and namespace = " + ns);
			Console.BackgroundColor = defaultColor;

			if (name.Equals("Order") && ns.Equals("http://orders/"))
			{
				Console.BackgroundColor = ConsoleColor.Blue;
				Console.WriteLine();
				Console.WriteLine("\t\tOrderExtension matched for Order!");
				Console.WriteLine();
				Console.BackgroundColor = defaultColor;

				compileUnit.ReferencedAssemblies.Add("Order.dll");
				codeNamespace.Imports.Add(new CodeNamespaceImport("Microsoft.Samples"));
				return "Order";
			}


			return null;
		}
	}
}
