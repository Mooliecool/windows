// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved.

using System;
using System.Xml;
using System.Xml.Schema;
using System.Xml.Serialization;
using System.Xml.Serialization.Advanced;
using System.CodeDom;
using System.CodeDom.Compiler;

namespace Microsoft.Samples.Xml.Serialization.SchemaImporterExtension
{
	public class ImportInteger 
		: System.Xml.Serialization.Advanced.SchemaImporterExtension
	{
		public override string ImportSchemaType(
			string name, 
			string ns, 
			XmlSchemaObject context, 
			XmlSchemas schemas, 
			XmlSchemaImporter importer, 
			CodeCompileUnit compileUnit, 
			CodeNamespace mainNamespace, 
			CodeGenerationOptions options, 
			CodeDomProvider codeProvider
		)
		{
			if (XmlSchema.Namespace == ns)
			{
				switch (name)
				{
					case "integer":
						return typeof(System.Int64).FullName;
					case "negativeInteger":
						return typeof(System.Int64).FullName;
					case "nonNegativeInteger":
						return typeof(System.UInt64).FullName;
					case "nonPositiveInteger":
						return typeof(System.Int64).FullName;
					case "positiveInteger":
						return typeof(System.UInt64).FullName;
					default:
						return null;
				}
			}
			else
			{
				return null;
			}
		}
	}
}
