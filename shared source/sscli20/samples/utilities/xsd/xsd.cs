//------------------------------------------------------------------------------
// <copyright file="xsd.cs" company="Microsoft">
//     
//      Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//     
//      The use and distribution terms for this software are contained in the file
//      named license.txt, which can be found in the root of this distribution.
//      By using this software in any fashion, you are agreeing to be bound by the
//      terms of this license.
//     
//      You must not remove this notice, or any other, from this software.
//     
// </copyright>                                                                
//------------------------------------------------------------------------------

/*+==========================================================================  
  Summary:  Utility to support XSD schema files; imports data types, creates
            new schemas, and converts the XDR format to the XSD format
==========================================================================+*/

namespace XsdTool {

    using System;
    using System.Reflection;
    using System.Xml.Serialization;
    using System.Xml.Schema;
    using System.IO;
    using System.CodeDom.Compiler;
    using System.Collections;
    using System.Threading;
    using System.Xml;
    using System.CodeDom;
    using System.Text;
    using Microsoft.DevApps.WebServices.XsdResources;


    public class Xsd {

        string TypeNameOf(object o) {
            if (o == null) return "null";
            return o.GetType().Name;
        }

        string NodeNameOf(XmlNodeEventArgs e) {
            switch (e.NodeType) {
                case XmlNodeType.Element: return "<" + e.LocalName + " xmlns=\"" + e.NamespaceURI + "\">";
                case XmlNodeType.Attribute: return "attribute: " + e.LocalName;
                case XmlNodeType.Text: return "text: " + e.Text;
                case XmlNodeType.CDATA: return "CDATA";
                case XmlNodeType.ProcessingInstruction: return "<?";
                case XmlNodeType.Comment: return "<!-- " + e.Text + " -->";
                default: throw new InvalidOperationException(Res.GetString(Res.ErrUnknownNodeType));
            }
        }

        void OnUnknownNode(object sender, XmlNodeEventArgs e) {
            if (e.NodeType != XmlNodeType.Comment)
                Console.WriteLine(Res.GetString(Res.Warning, Res.GetString(Res.UnhandledNode, e.LineNumber, e.LinePosition, NodeNameOf(e))));
        }

        void AddEvents(XmlSerializer serializer) {
            serializer.UnknownNode += new XmlNodeEventHandler(this.OnUnknownNode);
        }

        TextWriter CreateOutputWriter(string outputdir, string fileName, string newExtension) {
            string strippedName = Path.GetFileName(fileName);
            string newExtensionName = Path.ChangeExtension(strippedName, newExtension);
            string outputName = Path.Combine(outputdir, newExtensionName);

            Console.Out.WriteLine(Res.GetString(Res.InfoWrittingFile, outputName));
            return new StreamWriter(outputName, false, new System.Text.UTF8Encoding(true));
        }

        // Get a code generator for the specified language. language can either be a known abbreviation
        // for C#, VB or JScript. Or it can be a fully-qualified (with assembly) name for an ICodeGenerator
        // or a CodeDomProvider.
        void CreateCodeGenerator(string language, ref ICodeGenerator codeGen, ref string fileExtension) {            
            CodeDomProvider codeProvider = null;
            if ((string.Compare(language, "CSharp", true) == 0) ||
                 (string.Compare(language, "C#", true) == 0) ||
                 (string.Compare(language, "CS", true) == 0)) {
                codeProvider = new Microsoft.CSharp.CSharpCodeProvider();
            }
            else if (string.Compare(language, "VB", true) == 0) {
                throw new Exception("VisualBasic not supported in the SSCLI");
            }
            else if ((string.Compare(language, "js", true) == 0) ||
                      (string.Compare(language, "jscript", true) == 0)) {
                Type t = Type.GetType("Microsoft.JScript.JScriptCodeProvider, " + AssemblyRef.MicrosoftJScript);
                codeProvider = (CodeDomProvider) Activator.CreateInstance(t);
            }
            else {
                //try to reflect a custom code generator
                //ignore case when reflecting; language argument must specify the namespace
                Type t = Type.GetType(language, false, true);
                if (t == null) {
                    throw new InvalidOperationException(Res.GetString(Res.ErrLanguage, language));
                }
                object o = Activator.CreateInstance(t);
                if (o is CodeDomProvider)
                    codeProvider = (CodeDomProvider)o;
                else if (o is ICodeGenerator)
                    codeGen = (ICodeGenerator)o;
                else {
                    throw new InvalidOperationException(Res.GetString(Res.ErrLanguage, language));
                }
            }

            if (codeProvider != null) {
                codeGen = codeProvider.CreateGenerator();
                fileExtension = codeProvider.FileExtension;
                if (fileExtension == null) 
                    fileExtension = string.Empty;
                else if (fileExtension.Length > 0 && fileExtension[0] != '.')
                    fileExtension = "." + fileExtension;
            }
            else
                fileExtension = ".src";        
        }

        private static XmlSchema ReadSchema(string location, bool useSerializer) {
            if (!File.Exists(location)) {
                throw new FileNotFoundException(Res.GetString(Res.FileNotFound, location));
            }
            XmlTextReader r = new XmlTextReader(location);
            if (useSerializer) {
                return (XmlSchema)(new XmlSerializer(typeof(XmlSchema))).Deserialize(r);
            }
            else {
                return XmlSchema.Read(r, null);
            }
        }

        void ImportSchemasAsClasses(
            string outputdir,
            ICodeGenerator codeGen,
            string fileExtension,
            IList fileNames, 
            string ns, 
            string uri, 
            IList elements) {

            XmlSchemas schemasToCompile = new XmlSchemas();
            XmlSchemas userSchemas = new XmlSchemas();
            string outputSchemaName = "";
            // Create parent schema
            XmlSchema parent = new XmlSchema();
            foreach (string fileName in fileNames) {
                schemasToCompile.Add(ReadSchema(fileName, false));
                userSchemas.Add(ReadSchema(fileName, true));
                outputSchemaName += Path.ChangeExtension(fileName,"").Replace('.','_');
            }

            Hashtable includeSchemas = new Hashtable();
            foreach (XmlSchema schema in schemasToCompile) {
                CollectIncludes(schema, includeSchemas, false);
            }
            Compile(schemasToCompile);

            includeSchemas = new Hashtable();
            foreach (XmlSchema schema in userSchemas) {
                CollectIncludes(schema, includeSchemas, true);
            }

            try {
                outputSchemaName = outputSchemaName.Substring(0, outputSchemaName.Length - 1);
                XmlSchemaImporter schemaImporter = new XmlSchemaImporter(userSchemas);
                CodeCompileUnit codeCompileUnit = new CodeCompileUnit();
                CodeNamespace codeNamespace = new CodeNamespace(ns);
                codeCompileUnit.Namespaces.Add(codeNamespace);
                GenerateVersionComment(codeNamespace);
                XmlCodeExporter codeExporter = new XmlCodeExporter(codeNamespace, codeCompileUnit);
                AddImports(codeNamespace, GetNamespacesForTypes(new Type[] { typeof(XmlAttributeAttribute) }));

                for (int i = 0; i < userSchemas.Count; i++) {
                    XmlSchema schema = userSchemas[i];
                    for (int j = 0; j < schema.Items.Count; j++) {
                        object item = schema.Items[j];
                        if (item is XmlSchemaElement) {
                            XmlSchemaElement element = (XmlSchemaElement)item;
                            if (!element.IsAbstract) {

                                if (uri.Length == 0 ||
                                    schema.TargetNamespace == uri) {

                                    bool found;
                                    if (elements.Count == 0) {
                                        found = true;
                                    }
                                    else {
                                        found = false;
                                        foreach (string e in elements) {
                                            if (e == element.Name) {
                                                found = true;
                                                break;
                                            }
                                        }
                                    }
                                    if (found) {
                                        XmlTypeMapping xmlTypeMapping = schemaImporter.ImportTypeMapping(new XmlQualifiedName(element.Name, schema.TargetNamespace));
                                        codeExporter.ExportTypeMapping(xmlTypeMapping);
                                    }
                                }
                            }
                        }
                    }
                }

                CodeTypeDeclarationCollection classes = codeNamespace.Types;
                if (classes == null || classes.Count == 0) {
                    Console.WriteLine(Res.GetString(Res.NoClassesGenerated));
                }
                else {
                    TextWriter writer = CreateOutputWriter(outputdir, outputSchemaName, fileExtension);
                    codeGen.GenerateCodeFromCompileUnit(codeCompileUnit, writer, null);
                    writer.Close();
                }
            }
            catch (Exception e) {
                throw new InvalidOperationException(Res.GetString(Res.ErrGenerateClassesForSchema, outputSchemaName), e);
            }
        }

        internal class Namespace {
            public const string SoapEncoding = "http://schemas.xmlsoap.org/soap/encoding/";
            public const string Wsdl = "http://schemas.xmlsoap.org/wsdl/";
            public const string ReservedXmlNs = "http://www.w3.org/XML/1998/namespace";
        }

        static void AddFakeSchemas(XmlSchema parent, XmlSchemas schemas) {
            if (schemas[XmlSchema.Namespace] == null) {
                XmlSchemaImport import = new XmlSchemaImport();
                import.Namespace = XmlSchema.Namespace;
                import.Schema = CreateFakeXsdSchema(XmlSchema.Namespace, "schema");
                parent.Includes.Add(import);
            }

            if (schemas[Namespace.SoapEncoding] == null) {
                XmlSchemaImport import = new XmlSchemaImport();
                import.Namespace = Namespace.SoapEncoding;
                import.Schema = CreateFakeSoapEncodingSchema(Namespace.SoapEncoding, "Array");
                parent.Includes.Add(import);
            }

            if (schemas[Namespace.Wsdl] == null) {
                XmlSchemaImport import = new XmlSchemaImport();
                import.Namespace = Namespace.Wsdl;
                import.Schema = CreateFakeWsdlSchema(Namespace.Wsdl);
                parent.Includes.Add(import);
            }
        }
        private static XmlSchema CreateFakeXsdSchema(string ns, string name) {

            /* Create fake xsd schema to fool the XmlSchema.Compiler
                <xsd:schema targetNamespace="http://www.w3.org/2001/XMLSchema" xmlns:xsd="http://www.w3.org/2001/XMLSchema">
                  <xsd:element name="schema">
                    <xsd:complexType />
                  </xsd:element>
                </xsd:schema>
            */

            XmlSchema schema = new XmlSchema();
            schema.TargetNamespace = ns;
            XmlSchemaElement element = new XmlSchemaElement();
            element.Name = name;
            XmlSchemaComplexType type = new XmlSchemaComplexType();
            element.SchemaType = type;
            schema.Items.Add(element);
            return schema;
        }

        private static XmlSchema CreateFakeSoapEncodingSchema(string ns, string name) {

            // Create soap encoding schema 

            XmlSchema schema = new XmlSchema();
            schema.TargetNamespace = ns;

            // Add Array type:
            /*
              <xs:group name="Array">
                <xs:sequence>
                  <xs:any minOccurs="0" maxOccurs="unbounded" namespace="##any" processContents="lax" />
                </xs:sequence>
              </xs:group>
              <xs:element name="Array" type="tns:Array" />
              <xs:complexType name="Array">
                <xs:group minOccurs="0" ref="tns:Array" />
              </xs:complexType>
            */

            XmlSchemaGroup group = new XmlSchemaGroup();
            group.Name = "Array";
            XmlSchemaSequence seq = new XmlSchemaSequence();
            XmlSchemaAny any = new XmlSchemaAny();
            any.MinOccurs = 0;
            any.MaxOccurs = decimal.MaxValue;
            seq.Items.Add(any);
            any.Namespace = "##any";
            any.ProcessContents = XmlSchemaContentProcessing.Lax;

            group.Particle = seq;
            schema.Items.Add(group);

            XmlSchemaComplexType type = new XmlSchemaComplexType();
            type.Name = name;
            XmlSchemaGroupRef qroupRef = new XmlSchemaGroupRef();
            qroupRef.RefName = new XmlQualifiedName("Array", ns);
            type.Particle = qroupRef;
            XmlSchemaAttribute attribute = new XmlSchemaAttribute();
            attribute.RefName = new XmlQualifiedName("arrayType", ns);
            type.Attributes.Add(attribute);
            schema.Items.Add(type);

            attribute = new XmlSchemaAttribute();
            attribute.Use = XmlSchemaUse.None;
            attribute.Name = "arrayType";
            schema.Items.Add(attribute);

            AddSimpleType(schema, "base64", "base64Binary");

            // Add all types derived from the primitive XSD types

            AddElementAndType(schema, "anyURI", ns);
            AddElementAndType(schema, "base64Binary", ns);
            AddElementAndType(schema, "boolean", ns);
            AddElementAndType(schema, "byte", ns);
            AddElementAndType(schema, "date", ns);
            AddElementAndType(schema, "dateTime", ns);
            AddElementAndType(schema, "decimal", ns);
            AddElementAndType(schema, "double", ns);
            AddElementAndType(schema, "duration", ns);
            AddElementAndType(schema, "ENTITIES", ns);
            AddElementAndType(schema, "ENTITY", ns);
            AddElementAndType(schema, "float", ns);
            AddElementAndType(schema, "gDay", ns);
            AddElementAndType(schema, "gMonth", ns);
            AddElementAndType(schema, "gMonthDay", ns);
            AddElementAndType(schema, "gYear", ns);
            AddElementAndType(schema, "gYearMonth", ns);
            AddElementAndType(schema, "hexBinary", ns);
            AddElementAndType(schema, "ID", ns);
            AddElementAndType(schema, "IDREF", ns);
            AddElementAndType(schema, "IDREFS", ns);
            AddElementAndType(schema, "int", ns);
            AddElementAndType(schema, "integer", ns);
            AddElementAndType(schema, "language", ns);
            AddElementAndType(schema, "long", ns);
            AddElementAndType(schema, "Name", ns);
            AddElementAndType(schema, "NCName", ns);
            AddElementAndType(schema, "negativeInteger", ns);
            AddElementAndType(schema, "NMTOKEN", ns);
            AddElementAndType(schema, "NMTOKENS", ns);
            AddElementAndType(schema, "nonNegativeInteger", ns);
            AddElementAndType(schema, "nonPositiveInteger", ns);
            AddElementAndType(schema, "normalizedString", ns);
            AddElementAndType(schema, "positiveInteger", ns);
            AddElementAndType(schema, "QName", ns);
            AddElementAndType(schema, "short", ns);
            AddElementAndType(schema, "string", ns);
            AddElementAndType(schema, "time", ns);
            AddElementAndType(schema, "token", ns);
            AddElementAndType(schema, "unsignedByte", ns);
            AddElementAndType(schema, "unsignedInt", ns);
            AddElementAndType(schema, "unsignedLong", ns);
            AddElementAndType(schema, "unsignedShort", ns);

            return schema;
        }

        private static void AddSimpleType(XmlSchema schema, string typeName, string baseXsdType) {
            XmlSchemaSimpleType type = new XmlSchemaSimpleType();
            type.Name = typeName;
            XmlSchemaSimpleTypeRestriction restriction = new XmlSchemaSimpleTypeRestriction();
            restriction.BaseTypeName = new XmlQualifiedName(baseXsdType, XmlSchema.Namespace);
            type.Content = restriction;
            schema.Items.Add(type);
        }

        private static void AddElementAndType(XmlSchema schema, string baseXsdType, string ns) {
            // name the element and soap-encoding type the same as the base XSD type
            XmlSchemaElement el = new XmlSchemaElement();
            el.Name = baseXsdType;
            el.SchemaTypeName = new XmlQualifiedName(baseXsdType, ns);
            schema.Items.Add(el);

            XmlSchemaComplexType type = new XmlSchemaComplexType();
            type.Name = baseXsdType;
            XmlSchemaSimpleContent model = new XmlSchemaSimpleContent();
            type.ContentModel = model;

            XmlSchemaSimpleContentExtension ex = new XmlSchemaSimpleContentExtension();
            ex.BaseTypeName = new XmlQualifiedName(baseXsdType, XmlSchema.Namespace);
            model.Content = ex;
            schema.Items.Add(type);
        }

        private static XmlSchema CreateFakeWsdlSchema(string ns) {
            XmlSchema schema = new XmlSchema();
            schema.TargetNamespace = ns;

            XmlSchemaAttribute attribute = new XmlSchemaAttribute();
            attribute.Use = XmlSchemaUse.None;
            attribute.Name = "arrayType";
            attribute.SchemaTypeName = new XmlQualifiedName("QName", XmlSchema.Namespace);
            schema.Items.Add(attribute);

            return schema;
        }

        private static void ValidationCallbackWithErrorCode (object sender, ValidationEventArgs args) {
            Console.WriteLine(Res.GetString(Res.SchemaValidationWarningDetails, args.Message));
        }

        private static void CollectIncludes(XmlSchema schema, Hashtable includeSchemas, bool useSerializer) {
            foreach(XmlSchemaExternal include in schema.Includes) {
                string schemaLocation = include.SchemaLocation;
                if (include is XmlSchemaImport) {
                    include.SchemaLocation = null;
                }
                else {
                    if (schemaLocation != null && schemaLocation.Length > 0) {
                        string fullPath = Path.GetFullPath(schemaLocation).ToLower();
                        if (includeSchemas[fullPath] == null) {
                            XmlSchema s = ReadSchema(schemaLocation, useSerializer);
                            includeSchemas[fullPath] = s;
                            CollectIncludes(s, includeSchemas, useSerializer);
                        }
                        include.Schema = (XmlSchema)includeSchemas[fullPath];
                        include.SchemaLocation = null;
                    }
                }
            }
        }

        private static void Compile(XmlSchemas userSchemas) {
            XmlSchema parent = new XmlSchema();
            foreach (XmlSchema s in userSchemas) {
                if (s.TargetNamespace != null && s.TargetNamespace.Length == 0) {
                    s.TargetNamespace = null;
                }

                if (s.TargetNamespace == parent.TargetNamespace) {
                    XmlSchemaInclude include = new XmlSchemaInclude();
                    include.Schema = s;
                    parent.Includes.Add(include);
                }
                else {
                    XmlSchemaImport import = new XmlSchemaImport();
                    import.Namespace = s.TargetNamespace;
                    import.Schema = s;
                    parent.Includes.Add(import);
                }
            }
            /*  to be able to compile multiple schemas using DataSets and encoded Arrays we need three additional schemas : 
                     XmlSchema:     <xs:schema targetNamespace="http://www.w3.org/2001/XMLSchema"..
                     SoapEncoding:  <xs:schema targetNamespace="http://schemas.xmlsoap.org/soap/encoding/"..
                     Wsdl:          <xs:schema targetNamespace="http://schemas.xmlsoap.org/wsdl/"..

                we do not need them for our validation, just to fool XmlSchema.Compile, so we are going to create the bare minimum schemas
            */

            AddFakeSchemas(parent, userSchemas);

            try {
                XmlSchemaCollection xsc = new XmlSchemaCollection();
                xsc.ValidationEventHandler += new ValidationEventHandler (ValidationCallbackWithErrorCode);
                xsc.Add(parent);
                
                if (xsc.Count == 0) {
                    Console.WriteLine(Environment.NewLine + Res.GetString(Res.SchemaValidationWarning) + Environment.NewLine);
                }
            }
            catch (Exception e) {
                Console.WriteLine(Environment.NewLine + Res.GetString(Res.SchemaValidationWarning) + Environment.NewLine + e.Message + Environment.NewLine);
            }       
        }

        private static void GenerateVersionComment(CodeNamespace codeNamespace) {
            codeNamespace.Comments.Add(new CodeCommentStatement(""));
            AssemblyName assemblyName = Assembly.GetExecutingAssembly().GetName();
            Version version = System.Environment.Version;
            codeNamespace.Comments.Add(new CodeCommentStatement(
            Res.GetString(Res.InfoVersionComment, assemblyName.Name, version.ToString())));
            codeNamespace.Comments.Add(new CodeCommentStatement(""));
        }



        public static void AddImports(CodeNamespace codeNamespace, string[] namespaces) {
            foreach (string ns in namespaces)
                codeNamespace.Imports.Add(new CodeNamespaceImport(ns));
        }



        public static string[] GetNamespacesForTypes(Type[] types) {
            Hashtable names = new Hashtable();
            for (int i = 0; i < types.Length; i++) {
                string name = types[i].FullName;
                int dot = name.LastIndexOf('.');
                if (dot > 0)
                    names[name.Substring(0, dot)] = types[i];
            }
            string[] ns = new string[names.Keys.Count];
            names.Keys.CopyTo(ns, 0);
            return ns;
        }
       

        void ExportSchemas(string outputdir, IList dlls, IList typeNames) {
            XmlReflectionImporter importer = new XmlReflectionImporter();
            XmlSchemas schemas = new XmlSchemas();
            XmlSchemaExporter exporter = new XmlSchemaExporter(schemas);

            foreach (string dll in dlls) {
                Assembly a = Assembly.LoadFrom(dll);
                if (a == null)
                    throw new InvalidOperationException(Res.GetString(Res.ErrLoadAssembly, dll));

                try {
                    foreach (Type type in a.GetTypes()) {
                        if (!type.IsPublic)
                            continue;

                        if (type.IsInterface)
                            continue;

                        bool found;
                        if (typeNames.Count == 0) {
                            found = true;
                        }
                        else {
                            found = false;
                            foreach (string typeName in typeNames) {
                                if (type.FullName == typeName ||
                                    type.Name == typeName ||
                                    (typeName.EndsWith(".*") && 
                                    type.FullName.StartsWith(typeName.Substring(0, typeName.Length - 2)))) {
                                    found = true;
                                    break;
                                }
                            }
                        }

                        if (found) {
                            XmlTypeMapping xmlTypeMapping = importer.ImportTypeMapping(type);
                            exporter.ExportTypeMapping(xmlTypeMapping);
                        }
                    }
                }
                catch (Exception e) {
                    throw new InvalidOperationException(Res.GetString(Res.ErrGeneral, dll), e);
                }
            }

            for (int i = 0; i < schemas.Count; i++) {
                XmlSchema schema = schemas[i];
                try {
                    TextWriter writer = CreateOutputWriter(outputdir, "schema" + i.ToString(), ".xsd");
                    schemas[i].Write(writer);
                    writer.Close();
                }
                catch (Exception e) {
                    throw new InvalidOperationException(Res.GetString(Res.ErrGeneral, schema.TargetNamespace), e);
                }
            }
        }


        public static int Main(string[] args) {
            Xsd xsd = new Xsd();
            return xsd.Run(args);
        }

        //Function is used when an argument has 2+ letters short form.
        bool ArgumentMatchEx(string arg, string formal, int minletters) {
            //arg comes on '/xxx' form, formal in the 'xxx' one
            if (arg.Length > minletters + 1) {
                return ArgumentMatch(arg, formal);
            }
            else if (arg.Length == minletters + 1) {
                return (string.Compare(arg, 1, formal, 0, minletters) == 0);
            }
            return false;
        }

        // assumes all same case.        
        bool ArgumentMatch(string arg, string formal) {
            if (arg[0] != '/' && arg[0] != '-') {
                return false;
            }
            arg = arg.Substring(1);
            return (arg == formal || (arg.Length == 1 && arg[0] == formal[0]));
        }

        int Run(string[] args) {
            
            try {
            bool classesGen = false;
            bool dataSetGen = false;
            bool writeHeader = true;
            ArrayList elements = new ArrayList();
            //default language is C#
            string language = "c#";
            string ns = string.Empty;
            string outputdir = string.Empty;
            ArrayList typeNames = new ArrayList();
            string uri = "";

            ArrayList instances = new ArrayList();
            ArrayList schemas = new ArrayList();
            ArrayList xdrSchemas = new ArrayList();
            ArrayList dlls = new ArrayList();

            for (int i = 0; i < args.Length; i++) {
                string arg = args[i];
                string value = string.Empty;
                bool argument = false;

#if !PLATFORM_UNIX
                if (arg.StartsWith("/") || arg.StartsWith("-")) {
                    argument = true;
                }
#else
                if (arg.StartsWith("/")) {
                    if ((arg.IndexOf(":") >= 0) ||
                            ((arg.IndexOf("/", 1) < 0) && (arg.IndexOf("\\", 1) < 0))) {
                        argument = true;
                    }
                }
                else
                if (arg.StartsWith("-")) {
                    argument = true;
                }
#endif                
                
                if (argument) {
                    int colonPos = arg.IndexOf(":");
                    if (colonPos != -1) {
                        value = arg.Substring(colonPos + 1);
                        arg = arg.Substring(0, colonPos);
                    }
                }
                
                arg = arg.ToLower();

                //the user may have wanted to provide an absolute path to the file, so detect that FIRST
                //for example. "c:bob.xsd" will be (erroneously) split up into arg = "c" and value = "bob.xsd"
                //however, "bob.xsd" will be "properly" split up into arg = "bob.xsd" and value = ""

                if (!argument && arg.EndsWith(".xsd")) {
                    schemas.Add(args[i]);
                }
                else if (!argument && arg.EndsWith(".xdr")) {
                    xdrSchemas.Add(args[i]);
                }
                else if (!argument && arg.EndsWith(".xml")) {
                    instances.Add(args[i]);
                }
                else if (!argument && (arg.EndsWith(".dll") || arg.EndsWith(".exe"))) {
                    dlls.Add(args[i]);
                }
                else if (ArgumentMatch(arg, "?") || ArgumentMatch(arg, "help")) {
                    WriteHeader();
                    WriteHelp();
                    return 0;
                }
                else if (ArgumentMatch(arg, "classes")) {
                    if (value.Length > 0) {
                        WriteHeader();
                        throw new InvalidOperationException(Res.GetString(Res.ErrInvalidArgument, arg + ":" + value));
                    }
                    classesGen = true;
                }
                else if (ArgumentMatch(arg, "dataset")) {
                    if (value.Length > 0) {
                        WriteHeader();
                        throw new InvalidOperationException(Res.GetString(Res.ErrInvalidArgument, arg + ":" + value));
                    }
                    dataSetGen = true;
                }
                else if (ArgumentMatch(arg, "element")) {
                    elements.Add(value);
                }
                else if (ArgumentMatch(arg, "language")) {
                    language = value.ToLower();
                }
                else if (ArgumentMatch(arg, "namespace")) {
                    ns = value;
                }
                else if (ArgumentMatch(arg, "nologo")) {
                    if (value.Length > 0) {
                        WriteHeader();
                        throw new InvalidOperationException(Res.GetString(Res.ErrInvalidArgument, arg + ":" + value));
                    }
                    writeHeader = false;
                }
                else if (ArgumentMatch(arg, "out") || ArgumentMatch(arg, "outputdir")) {
                    outputdir = value;
                }
                else if (ArgumentMatch(arg, "type")) {
                    typeNames.Add(value);
                }
                else if (ArgumentMatch(arg, "uri")) {
                    uri = value;
                }
                else {
                    WriteHeader();
                    throw new InvalidOperationException(Res.GetString(Res.ErrInvalidArgument, args[i]));
                }
            }

            if (writeHeader)
                WriteHeader();

            bool schemasFound = schemas.Count > 0;
            bool xdrSchemasFound = xdrSchemas.Count > 0;
            bool dllsFound = dlls.Count > 0;
            bool instancesFound = instances.Count > 0;
            
            if (outputdir.Length == 0) {
                outputdir = Directory.GetCurrentDirectory();
            }
            if (schemasFound) {
                if (dllsFound || instancesFound || xdrSchemasFound) {
                    throw new InvalidOperationException(Res.GetString(Res.ErrInputFileTypes));
                }
                if (classesGen == dataSetGen) {
                    throw new InvalidOperationException(Res.GetString(Res.ErrClassOrDataset));
                }

                ICodeGenerator codeGen = null;
                string fileExtension = string.Empty;

                CreateCodeGenerator(language, ref codeGen, ref fileExtension);

                if (classesGen)
                    ImportSchemasAsClasses(outputdir, codeGen, fileExtension, schemas, ns, uri, elements);
                else
                    throw new Exception("DataSet import not supported in the SSCLI");
            }
            else if (dllsFound) {
                if (instancesFound || xdrSchemasFound)
                    throw new InvalidOperationException(Res.GetString(Res.ErrInputFileTypes));
                ExportSchemas(outputdir, dlls, typeNames);
            }
            else if (xdrSchemasFound) {
                if (instancesFound)
                    throw new InvalidOperationException(Res.GetString(Res.ErrInputFileTypes));
                throw new Exception("XDR schema conversion not supported in the SSCLI");
            }
            else if (instancesFound) {
                throw new Exception("Schema inference not supported in the SSCLI");
            }
            else {
                WriteHelp();
                return 0;
            }
            }
            catch (Exception e) {
                Error(e, Res.GetString(Res.Error));
                return 1;
            }
            return 0;
        }

        static void Error(Exception e, string prefix) {
            Console.Error.WriteLine(prefix + e.Message);
            if (e.InnerException != null) {
                Error(e.InnerException, "  - ");
            }
            else {
                Console.WriteLine(Res.GetString(Res.MoreHelp,"/?"));
            }
        }

        private void WriteHeader() {
        object[] 
            attributes=Assembly.GetEntryAssembly().GetCustomAttributes(typeof(AssemblyProductAttribute) ,true);
            AssemblyProductAttribute   productAttribute  = (AssemblyProductAttribute) attributes[0];
            
            Console.WriteLine(Res.GetString(Res.Logo, 
                                            productAttribute.Product,
                                            ThisAssembly.InformationalVersion,
                                            ThisAssembly.Copyright));
        }

        void WriteHelp() {
            Console.WriteLine(Res.GetString(Res.HelpDescription, ThisAssembly.Title));
            Console.WriteLine(Res.GetString(Res.HelpUsage, ThisAssembly.Title));
            Console.WriteLine(Res.GetString(Res.HelpOptions));
            Console.WriteLine(Res.GetString(Res.HelpClasses, "/classes", "/c"));
            Console.WriteLine(Res.GetString(Res.HelpElement, "/element:", "/e:"));
            Console.WriteLine(Res.GetString(Res.HelpLanguage, "/language:", "/l:"));
            Console.WriteLine(Res.GetString(Res.HelpNamespace, "/namespace:", "/n:"));
            Console.WriteLine(Res.GetString(Res.HelpNoLogo, "/nologo"));
            Console.WriteLine(Res.GetString(Res.HelpOut, "/out:", "/o:"));
            Console.WriteLine(Res.GetString(Res.HelpType,"/type:","/t:"));
            Console.WriteLine(Res.GetString(Res.HelpUri, "/uri:", "/u:"));
            Console.WriteLine(Res.GetString(Res.HelpArguments));
            Console.WriteLine(Res.GetString(Res.HelpArgumentsDescription));
        }
    }
}

