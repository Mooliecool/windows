//------------------------------------------------------------------------------
// <copyright file="Compilation.cs" company="Microsoft">
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
// <owner current="true" primary="true">ElenaK</owner>                                                                
//------------------------------------------------------------------------------

namespace System.Xml.Serialization {

    using System.Reflection;
    using System.Reflection.Emit;
    using System.Collections;
    using System.IO;
    using System;
    using System.Text;
    using System.Xml;
    using System.Threading;
    using System.Security;
    using System.Security.Permissions;
    using System.Security.Policy;
    using System.Xml.Serialization.Configuration;
    using System.Diagnostics;
    using System.CodeDom.Compiler;
    using System.Globalization;
    
    internal class TempAssembly {
        const string GeneratedAssemblyNamespace = "Microsoft.Xml.Serialization.GeneratedAssembly";
        Assembly assembly;
        bool pregeneratedAssmbly = false;
        XmlSerializerImplementation contract = null;
        Hashtable writerMethods;
        Hashtable readerMethods;
        TempMethodDictionary methods;
        static object[] emptyObjectArray = new object[0];
        Hashtable assemblies = new Hashtable();
        static FileIOPermission fileIOPermission;

        internal class TempMethod {
            internal MethodInfo writeMethod;
            internal MethodInfo readMethod;
            internal string name;
            internal string ns;
            internal bool isSoap;
            internal string methodKey;
        }

        private TempAssembly() {
        }

        internal TempAssembly(XmlMapping[] xmlMappings, Type[] types, string defaultNamespace, string location, Evidence evidence) {
            CompilerParameters parameters = new CompilerParameters();
            parameters.GenerateInMemory = true;
            TempFileCollection tempFiles = new TempFileCollection(location);
            parameters.TempFiles = tempFiles;
            assembly = GenerateAssembly(xmlMappings, types, defaultNamespace, evidence, parameters, null, assemblies);
#if DEBUG
            // use exception in the place of Debug.Assert to avoid throwing asserts from a server process such as aspnet_ewp.exe
            if (assembly == null) throw new InvalidOperationException(Res.GetString(Res.XmlInternalErrorDetails, "Failed to generate XmlSerializer assembly, but did not throw"));
#endif      
            InitAssemblyMethods(xmlMappings);
        }

        internal TempAssembly(XmlMapping[] xmlMappings, Assembly assembly, XmlSerializerImplementation contract) {
            this.assembly = assembly;
            InitAssemblyMethods(xmlMappings);
            this.contract = contract;
            pregeneratedAssmbly = true;
        }

        internal TempAssembly(XmlSerializerImplementation contract) {
            this.contract = contract;
            pregeneratedAssmbly = true;
        }

        internal XmlSerializerImplementation Contract { 
            get {
                if (contract == null) {
                    contract = (XmlSerializerImplementation)Activator.CreateInstance(GetTypeFromAssembly(this.assembly, "XmlSerializerContract"));
                }
                return contract; 
            } 
        }

        internal void InitAssemblyMethods(XmlMapping[] xmlMappings) {
            methods = new TempMethodDictionary();
            for (int i = 0; i < xmlMappings.Length; i++) {
                TempMethod method = new TempMethod();
                method.isSoap = xmlMappings[i].IsSoap;
                method.methodKey = xmlMappings[i].Key;
                XmlTypeMapping xmlTypeMapping = xmlMappings[i] as XmlTypeMapping;
                if (xmlTypeMapping != null) {
                    method.name = xmlTypeMapping.ElementName;
                    method.ns = xmlTypeMapping.Namespace;
                }
                methods.Add(xmlMappings[i].Key, method);
            }
        }

        /// <devdoc>
        ///    <para>
        ///    Attempts to load pre-generated serialization assembly.
        ///    First check for the [XmlSerializerAssembly] attribute
        ///    </para>
        /// </devdoc>
        internal static Assembly LoadGeneratedAssembly(Type type, string defaultNamespace, out XmlSerializerImplementation contract) {
            Assembly serializer = null;
            contract = null;
            string serializerName = null;
            bool logEnabled = DiagnosticsSwitches.PregenEventLog.Enabled;

            // check to see if we loading explicit pre-generated assembly
            object[] attrs = type.GetCustomAttributes(typeof(XmlSerializerAssemblyAttribute), false);
            if (attrs.Length == 0) {
                // Guess serializer name: if parent assembly signed use strong name 
                AssemblyName name = GetName(type.Assembly, true);
                serializerName = Compiler.GetTempAssemblyName(name, defaultNamespace);
                // use strong name 
                name.Name = serializerName;
                name.CodeBase = null;
                name.CultureInfo = CultureInfo.InvariantCulture;
                try {
                    serializer = Assembly.Load(name);
                }
                catch (Exception e) {
                    if (e is ThreadAbortException || e is StackOverflowException || e is OutOfMemoryException) {
                        throw;
                    }
                    byte[] token = name.GetPublicKeyToken();
                    if (token != null && token.Length > 0) {
                        // the parent assembly was signed, so do not try to LoadWithPartialName
                        return null;
                    }
#pragma warning disable 618
                    serializer = Assembly.LoadWithPartialName(serializerName, null);
#pragma warning restore 618
                }
                catch {
                    return null;
                }
                if (serializer == null) {
                    return null;
                }
                if (!IsSerializerVersionMatch(serializer, type, defaultNamespace, null)) {
                    return null;
                }
            }
            else {
                XmlSerializerAssemblyAttribute assemblyAttribute = (XmlSerializerAssemblyAttribute)attrs[0];
                if (assemblyAttribute.AssemblyName != null && assemblyAttribute.CodeBase != null)
                    throw new InvalidOperationException(Res.GetString(Res.XmlPregenInvalidXmlSerializerAssemblyAttribute, "AssemblyName", "CodeBase")); 

                // found XmlSerializerAssemblyAttribute attribute, it should have all needed information to load the pre-generated serializer
                if (assemblyAttribute.AssemblyName != null) {
                    serializerName = assemblyAttribute.AssemblyName;
#pragma warning disable 618
                    serializer = Assembly.LoadWithPartialName(serializerName, null);
#pragma warning restore 618
                }
                else if (assemblyAttribute.CodeBase != null && assemblyAttribute.CodeBase.Length > 0) {
                    serializerName = assemblyAttribute.CodeBase;
                    serializer = Assembly.LoadFrom(serializerName);
                }
                else {
                    serializerName = type.Assembly.FullName;
                    serializer = type.Assembly;
                }
                if (serializer == null) {
                    throw new FileNotFoundException(null, serializerName); 
                }
            }
            Type contractType = GetTypeFromAssembly(serializer, "XmlSerializerContract");
            contract = (XmlSerializerImplementation)Activator.CreateInstance(contractType);
            if (contract.CanSerialize(type))
                return serializer;

            return null;
        }
        

        static AssemblyName GetName(Assembly assembly, bool copyName) {
            PermissionSet perms = new PermissionSet(PermissionState.None);
            perms.AddPermission(new FileIOPermission(PermissionState.Unrestricted));
            perms.Assert();
            return assembly.GetName(copyName);
        }


        static bool IsSerializerVersionMatch(Assembly serializer, Type type, string defaultNamespace, string location) {
            if (serializer == null)
                return false;
            object[] attrs = serializer.GetCustomAttributes(typeof(XmlSerializerVersionAttribute), false);
            if (attrs.Length != 1)
                return false;

            XmlSerializerVersionAttribute assemblyInfo = (XmlSerializerVersionAttribute)attrs[0];
            if (assemblyInfo.ParentAssemblyId == GenerateAssemblyId(type) && assemblyInfo.Namespace == defaultNamespace)
                return true;
            return false;
        }
        static string GenerateAssemblyId(Type type) {
            Module[] modules = type.Assembly.GetModules();
            ArrayList list = new ArrayList();
            for (int i = 0; i < modules.Length; i++) {
                list.Add(modules[i].ModuleVersionId.ToString());
            }
            list.Sort();
            StringBuilder sb = new StringBuilder();
            for (int i = 0; i < list.Count; i++) {
                sb.Append(list[i].ToString());
                sb.Append(",");
            }
            return sb.ToString();
        }

        internal static Assembly GenerateAssembly(XmlMapping[] xmlMappings, Type[] types, string defaultNamespace, Evidence evidence, CompilerParameters parameters, Assembly assembly, Hashtable assemblies) {
            FileIOPermission.Assert();
            for (int i = 0; i < xmlMappings.Length; i++) {
                xmlMappings[i].CheckShallow();
            }
            Compiler compiler = new Compiler();
            try {
                Hashtable scopeTable = new Hashtable();
                foreach (XmlMapping mapping in xmlMappings)
                    scopeTable[mapping.Scope] = mapping;
                TypeScope[] scopes = new TypeScope[scopeTable.Keys.Count];
                scopeTable.Keys.CopyTo(scopes, 0);
                
                assemblies.Clear();
                Hashtable importedTypes = new Hashtable();
                foreach (TypeScope scope in scopes) {
                    foreach (Type t in scope.Types) {
                        compiler.AddImport(t, importedTypes);
                        Assembly a = t.Assembly;
                        string name = a.FullName;
                        if (assemblies[name] != null)
                            continue;
                        if (!a.GlobalAssemblyCache) {
                            assemblies[name] = a;
                        }
                    }
                }
                for (int i = 0; i < types.Length; i++) {
                    compiler.AddImport(types[i], importedTypes);
                }
                compiler.AddImport(typeof(object).Assembly);
                compiler.AddImport(typeof(XmlSerializer).Assembly);

                IndentedWriter writer = new IndentedWriter(compiler.Source, false);
                
                writer.WriteLine("#if _DYNAMIC_XMLSERIALIZER_COMPILATION");
                writer.WriteLine("[assembly:System.Security.AllowPartiallyTrustedCallers()]");
                writer.WriteLine("[assembly:System.Security.SecurityTransparent()]");
                writer.WriteLine("#endif");
                // Add AssemblyVersion attribute to match parent accembly version
                if (types != null && types.Length > 0 && types[0] != null) {
                    writer.WriteLine("[assembly:System.Reflection.AssemblyVersionAttribute(\"" + types[0].Assembly.GetName().Version.ToString() + "\")]");
                }
                if (assembly != null && types.Length > 0) {
                    for (int i = 0; i < types.Length; i++) {
                        Type type = types[i];
                        if (type == null)
                            continue;
                        if (DynamicAssemblies.IsTypeDynamic(type)) {
                            throw new InvalidOperationException(Res.GetString(Res.XmlPregenTypeDynamic, types[i].FullName));
                        }
                    }
                    writer.Write("[assembly:");
                    writer.Write(typeof(XmlSerializerVersionAttribute).FullName);
                    writer.Write("(");
                    writer.Write("ParentAssemblyId=");
                    ReflectionAwareCodeGen.WriteQuotedCSharpString(writer, GenerateAssemblyId(types[0]));
                    writer.Write(", Version=");
                    ReflectionAwareCodeGen.WriteQuotedCSharpString(writer, ThisAssembly.Version);
                    if (defaultNamespace != null) {
                        writer.Write(", Namespace=");
                        ReflectionAwareCodeGen.WriteQuotedCSharpString(writer, defaultNamespace);
                    }
                    writer.WriteLine(")]");
                }
                CodeIdentifiers classes = new CodeIdentifiers();
                classes.AddUnique("XmlSerializationWriter", "XmlSerializationWriter");
                classes.AddUnique("XmlSerializationReader", "XmlSerializationReader");
                string suffix = null;
                if (types != null && types.Length == 1 && types[0] != null) {
                    suffix = CodeIdentifier.MakeValid(types[0].Name);
                    if (types[0].IsArray) {
                        suffix += "Array";
                    }
                }

                writer.WriteLine("namespace " + GeneratedAssemblyNamespace + " {");
                writer.Indent++;

                writer.WriteLine();

                string writerClass = "XmlSerializationWriter" + suffix;
                writerClass = classes.AddUnique(writerClass, writerClass);
                XmlSerializationWriterCodeGen writerCodeGen = new XmlSerializationWriterCodeGen(writer, scopes, "public", writerClass);

                writerCodeGen.GenerateBegin();
                string[] writeMethodNames = new string[xmlMappings.Length];

                for (int i = 0; i < xmlMappings.Length; i++) {
                    writeMethodNames[i] = writerCodeGen.GenerateElement(xmlMappings[i]);
                }
                writerCodeGen.GenerateEnd();
                    
                writer.WriteLine();

                string readerClass = "XmlSerializationReader" + suffix;
                readerClass = classes.AddUnique(readerClass, readerClass);
                XmlSerializationReaderCodeGen readerCodeGen = new XmlSerializationReaderCodeGen(writer, scopes, "public", readerClass);

                readerCodeGen.GenerateBegin();
                string[] readMethodNames = new string[xmlMappings.Length];
                for (int i = 0; i < xmlMappings.Length; i++) {
                    readMethodNames[i] = readerCodeGen.GenerateElement(xmlMappings[i]);
                }
                readerCodeGen.GenerateEnd(readMethodNames, xmlMappings, types);

                string baseSerializer = readerCodeGen.GenerateBaseSerializer("XmlSerializer1", readerClass, writerClass, classes);
                Hashtable serializers = new Hashtable();
                for (int i = 0; i < xmlMappings.Length; i++) {
                    if (serializers[xmlMappings[i].Key] == null) {
                        serializers[xmlMappings[i].Key] = readerCodeGen.GenerateTypedSerializer(readMethodNames[i], writeMethodNames[i], xmlMappings[i], classes, baseSerializer, readerClass, writerClass);
                    }
                }
                readerCodeGen.GenerateSerializerContract("XmlSerializerContract", xmlMappings, types, readerClass, readMethodNames, writerClass, writeMethodNames, serializers);
                writer.Indent--;
                writer.WriteLine("}");

                return compiler.Compile(assembly, defaultNamespace, parameters, evidence);
            }
            finally {
                compiler.Close();
            }
        }

        static MethodInfo GetMethodFromType(Type type, string methodName, Assembly assembly) {
            MethodInfo method = type.GetMethod(methodName);
            if (method != null) 
                return method;

            MissingMethodException missingMethod = new MissingMethodException(type.FullName, methodName);
            if (assembly != null) {
                throw new InvalidOperationException(Res.GetString(Res.XmlSerializerExpired, assembly.FullName, assembly.CodeBase), missingMethod); 
            }
            throw missingMethod;
        }

        internal static Type GetTypeFromAssembly(Assembly assembly, string typeName) {
            typeName = GeneratedAssemblyNamespace + "." + typeName;
            Type type = assembly.GetType(typeName);
            if (type == null) throw new InvalidOperationException(Res.GetString(Res.XmlMissingType, typeName, assembly.FullName));
            return type;
        }

        internal bool CanRead(XmlMapping mapping, XmlReader xmlReader) {
            if (mapping == null)
                return false;

            if (mapping.Accessor.Any) {
                return true;
            }
            TempMethod method = methods[mapping.Key];
            return xmlReader.IsStartElement(method.name, method.ns);
        }

        string ValidateEncodingStyle(string encodingStyle, string methodKey) {
            if (encodingStyle != null && encodingStyle.Length > 0) {
                if (methods[methodKey].isSoap) {
                    if (encodingStyle != Soap.Encoding && encodingStyle != Soap12.Encoding) {
                        throw new InvalidOperationException(Res.GetString(Res.XmlInvalidEncoding3, encodingStyle, Soap.Encoding, Soap12.Encoding));
                    }
                }
                else {
                    throw new InvalidOperationException(Res.GetString(Res.XmlInvalidEncodingNotEncoded1, encodingStyle));
                }
            }
            else {
                if (methods[methodKey].isSoap) {
                    encodingStyle = Soap.Encoding;
                }
            }
            return encodingStyle;
        }

        internal static FileIOPermission FileIOPermission {
            get {
                if (fileIOPermission == null)
                    fileIOPermission = new FileIOPermission(PermissionState.Unrestricted);
                return fileIOPermission;
            }
        }

        internal object InvokeReader(XmlMapping mapping, XmlReader xmlReader, XmlDeserializationEvents events, string encodingStyle) {
            XmlSerializationReader reader = null;
            try {
                encodingStyle = ValidateEncodingStyle(encodingStyle, mapping.Key);
                reader = Contract.Reader;
                reader.Init(xmlReader, events, encodingStyle, this);
                if (methods[mapping.Key].readMethod == null) {
                    if (readerMethods == null) {
                        readerMethods = Contract.ReadMethods;
                    }
                    string methodName = (string)readerMethods[mapping.Key];
                    if (methodName == null) {
                        throw new InvalidOperationException(Res.GetString(Res.XmlNotSerializable, mapping.Accessor.Name));
                    }
                    methods[mapping.Key].readMethod = GetMethodFromType(reader.GetType(), methodName, pregeneratedAssmbly ? this.assembly : null);
                }
                return methods[mapping.Key].readMethod.Invoke(reader, emptyObjectArray);
            }
            catch (SecurityException e) {
                throw new InvalidOperationException(Res.GetString(Res.XmlNoPartialTrust), e);
            }
            finally {
                if (reader != null)
                    reader.Dispose();
            }
        }

        internal void InvokeWriter(XmlMapping mapping, XmlWriter xmlWriter, object o, XmlSerializerNamespaces namespaces, string encodingStyle, string id) {
            XmlSerializationWriter writer = null;
            try {
                encodingStyle = ValidateEncodingStyle(encodingStyle, mapping.Key);
                writer = Contract.Writer;
                writer.Init(xmlWriter, namespaces, encodingStyle, id, this);
                if (methods[mapping.Key].writeMethod == null) {
                    if (writerMethods == null) {
                        writerMethods = Contract.WriteMethods;
                    }
                    string methodName = (string)writerMethods[mapping.Key];
                    if (methodName == null) {
                        throw new InvalidOperationException(Res.GetString(Res.XmlNotSerializable, mapping.Accessor.Name));
                    }
                    methods[mapping.Key].writeMethod = GetMethodFromType(writer.GetType(), methodName, pregeneratedAssmbly ? assembly : null);
                }
                methods[mapping.Key].writeMethod.Invoke(writer, new object[] { o });
            }
            catch (SecurityException e) {
                throw new InvalidOperationException(Res.GetString(Res.XmlNoPartialTrust), e);
            }
            finally {
                if (writer != null)
                    writer.Dispose();
            }
        }

        internal Assembly GetReferencedAssembly(string name) {
            return assemblies != null && name != null ? (Assembly)assemblies[name] : null;
        }

        internal bool NeedAssembyResolve {
            get { return assemblies != null && assemblies.Count > 0; }
        }

        internal sealed class TempMethodDictionary : DictionaryBase {
            internal TempMethod this[string key] {
                get {
                    return (TempMethod) Dictionary[key];
                }
            }
            internal void Add(string key, TempMethod value) {
                Dictionary.Add(key, value);
            }
        }
    }

    class TempAssemblyCacheKey {
        string ns;
        object type;
        
        internal TempAssemblyCacheKey(string ns, object type) {
            this.type = type;
            this.ns = ns;
        }

        public override bool Equals(object o) {
            TempAssemblyCacheKey key = o as TempAssemblyCacheKey;
            if (key == null) return false;
            return (key.type == this.type && key.ns == this.ns);
        }

        public override int GetHashCode() {
            return ((ns != null ? ns.GetHashCode() : 0) ^ (type != null ? type.GetHashCode() : 0));
        }
    }

    internal class TempAssemblyCache {
        Hashtable cache = new Hashtable();

        internal TempAssembly this[string ns, object o] {
            get { return (TempAssembly)cache[new TempAssemblyCacheKey(ns, o)]; }
        }

        internal void Add(string ns, object o, TempAssembly assembly) {
            TempAssemblyCacheKey key = new TempAssemblyCacheKey(ns, o);
            lock(this) {
                if (cache[key] == assembly) return;
                Hashtable clone = new Hashtable();
                foreach (object k in cache.Keys) {
                    clone.Add(k, cache[k]);
                }
                cache = clone;
                cache[key] = assembly;
            }
        }
    }
}

