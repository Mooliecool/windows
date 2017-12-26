//------------------------------------------------------------------------------
// <copyright file="XmlSerializer.cs" company="Microsoft">
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
    using System.Collections;
    using System.IO;
    using System.Xml.Schema;
    using System;
    using System.Text;
    using System.Threading;
    using System.Globalization;
    using System.Security.Permissions;
    using System.Security.Policy;
    using System.Xml.Serialization.Configuration;
    using System.Diagnostics;
    using System.CodeDom.Compiler;


    /// <include file='doc\XmlSerializerFactory.uex' path='docs/doc[@for="XmlSerializerFactory"]/*' />
    /// <devdoc>
    ///    <para>[To be supplied.]</para>
    /// </devdoc>
    public class XmlSerializerFactory {
        static TempAssemblyCache cache = new TempAssemblyCache();

        /// <include file='doc\XmlSerializerFactory.uex' path='docs/doc[@for="XmlSerializerFactory.CreateSerializer"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlSerializer CreateSerializer(Type type, XmlAttributeOverrides overrides, Type[] extraTypes, XmlRootAttribute root, string defaultNamespace) {
            return CreateSerializer(type, overrides, extraTypes, root, defaultNamespace, null, null);
        }

        /// <include file='doc\XmlSerializerFactory.uex' path='docs/doc[@for="XmlSerializerFactory.CreateSerializer2"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlSerializer CreateSerializer(Type type, XmlRootAttribute root) {
            return CreateSerializer(type, null, new Type[0], root, null, null, null);
        }

        /// <include file='doc\XmlSerializerFactory.uex' path='docs/doc[@for="XmlSerializerFactory.CreateSerializer3"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlSerializer CreateSerializer(Type type, Type[] extraTypes) {
            return CreateSerializer(type, null, extraTypes, null, null, null, null);
        }

        /// <include file='doc\XmlSerializerFactory.uex' path='docs/doc[@for="XmlSerializerFactory.CreateSerializer4"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlSerializer CreateSerializer(Type type, XmlAttributeOverrides overrides) {
            return CreateSerializer(type, overrides, new Type[0], null, null, null, null);
        }

        /// <include file='doc\XmlSerializerFactory.uex' path='docs/doc[@for="XmlSerializerFactory.CreateSerializer5"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlSerializer CreateSerializer(XmlTypeMapping xmlTypeMapping) {
            TempAssembly tempAssembly = XmlSerializer.GenerateTempAssembly(xmlTypeMapping);
            return (XmlSerializer)tempAssembly.Contract.TypedSerializers[xmlTypeMapping.Key];
        }

        /// <include file='doc\XmlSerializerFactory.uex' path='docs/doc[@for="XmlSerializerFactory.CreateSerializer6"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlSerializer CreateSerializer(Type type) {
            return CreateSerializer(type, (string)null);
        }

        /// <include file='doc\XmlSerializerFactory.uex' path='docs/doc[@for="XmlSerializerFactory.CreateSerializer1"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlSerializer CreateSerializer(Type type, string defaultNamespace) {
            if (type == null)
                throw new ArgumentNullException("type");
            TempAssembly tempAssembly = cache[defaultNamespace, type];
            XmlTypeMapping mapping = null;
            if (tempAssembly == null) {
                lock (cache) {
                    tempAssembly = cache[defaultNamespace, type];
                    if (tempAssembly == null) {
                        XmlSerializerImplementation contract;
                        Assembly assembly = TempAssembly.LoadGeneratedAssembly(type, defaultNamespace, out contract);
                        if (assembly == null) {
                            // need to reflect and generate new serialization assembly
                            XmlReflectionImporter importer = new XmlReflectionImporter(defaultNamespace);
                            mapping = importer.ImportTypeMapping(type, null, defaultNamespace);
                            tempAssembly = XmlSerializer.GenerateTempAssembly(mapping, type, defaultNamespace);
                        }
                        else {
                            tempAssembly = new TempAssembly(contract);
                        }
                        cache.Add(defaultNamespace, type, tempAssembly);
                    }
                }
            }
            if (mapping == null) {
                mapping = XmlReflectionImporter.GetTopLevelMapping(type, defaultNamespace);
            }
            return (XmlSerializer)tempAssembly.Contract.GetSerializer(type);
        }

        /// <include file='doc\XmlSerializerFactory.uex' path='docs/doc[@for="XmlSerializerFactory.CreateSerializer7"]/*' />
        /// <devdoc>
        ///    <para>[To be supplied.]</para>
        /// </devdoc>
        public XmlSerializer CreateSerializer(Type type, XmlAttributeOverrides overrides, Type[] extraTypes, XmlRootAttribute root, string defaultNamespace, string location, Evidence evidence) {
            if (type == null)
                throw new ArgumentNullException("type");
            XmlReflectionImporter importer = new XmlReflectionImporter(overrides, defaultNamespace);
            for (int i = 0; i < extraTypes.Length; i++)
                importer.IncludeType(extraTypes[i]);
            XmlTypeMapping mapping = importer.ImportTypeMapping(type, root, defaultNamespace);
            TempAssembly tempAssembly = XmlSerializer.GenerateTempAssembly(mapping, type, defaultNamespace, location, evidence);
            return (XmlSerializer)tempAssembly.Contract.TypedSerializers[mapping.Key];
        }
    }
}
