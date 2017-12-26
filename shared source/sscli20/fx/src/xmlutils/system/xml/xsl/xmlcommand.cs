//------------------------------------------------------------------------------
// <copyright file="XmlCommand.cs" company="Microsoft">
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

using System.Collections;
using System.IO;
using System.Text;
using System.Xml.XPath;

namespace System.Xml.Xsl {

    /// <summary>
    /// Executable query object that is produced by a QilExpression -> Executable generator.  Implementations
    /// should be stateless so that Execute can be concurrently called by multiple threads.
    /// </summary>
    internal abstract class XmlCommand {
        /// <devdoc>
        ///     <para>
        ///         Executes the query over the provided XPathNavigator with the given XsltArgumentList
        ///         as run-time parameters. The results are output to the provided XmlWriter.
        ///     </para>
        /// </devdoc>
        public abstract void Execute(IXPathNavigable contextDocument, XmlResolver dataSources, XsltArgumentList argumentList, XmlWriter results);

        /// <devdoc>
        ///     <para>
        ///         Executes the query over the provided XPathNavigator with the given XsltArgumentList
        ///         as run-time parameters. The results are output to the provided TextWriter.
        ///     </para>
        /// </devdoc>
        public abstract void Execute(IXPathNavigable contextDocument, XmlResolver dataSources, XsltArgumentList argumentList, TextWriter results);

        /// <devdoc>
        ///     <para>
        ///         Executes the query over the provided XPathNavigator with the given XsltArgumentList
        ///         as run-time parameters. The results are output to the provided Stream.
        ///     </para>
        /// </devdoc>
        public abstract void Execute(IXPathNavigable contextDocument, XmlResolver dataSources, XsltArgumentList argumentList, Stream results);

        /// <devdoc>
        ///     <para>
        ///         Executes the query by accessing datasources via the XmlResolver and using
        ///         run-time parameters as provided by the XsltArgumentList. The default document
        ///         is mapped into the XmlResolver with the provided name. The results are output
        ///         to the provided XmlWriter.
        ///     </para>
        /// </devdoc>
        public abstract void Execute(XmlReader contextDocument, XmlResolver dataSources, XsltArgumentList argumentList, XmlWriter results);

        /// <devdoc>
        ///     <para>
        ///         Executes the query by accessing datasources via the XmlResolver and using
        ///         run-time parameters as provided by the XsltArgumentList. The default document
        ///         is mapped into the XmlResolver with the provided name. The results are output
        ///         to the provided TextWriter.
        ///     </para>
        /// </devdoc>
        public abstract void Execute(XmlReader contextDocument, XmlResolver dataSources, XsltArgumentList argumentList, TextWriter results);

        /// <devdoc>
        ///     <para>
        ///         Executes the query by accessing datasources via the XmlResolver and using
        ///         run-time parameters as provided by the XsltArgumentList. The default document
        ///         is mapped into the XmlResolver with the provided name. The results are output
        ///         to the provided Stream.
        ///     </para>
        /// </devdoc>
        public abstract void Execute(XmlReader contextDocument, XmlResolver dataSources, XsltArgumentList argumentList, Stream results);

        /// <devdoc>
        ///     <para>
        ///         Executes the query by accessing datasources via the XmlResolver and using
        ///         run-time parameters as provided by the XsltArgumentList. The default document
        ///         is mapped into the XmlResolver with the provided name. The results are returned
        ///         as an IList.
        ///     </para>
        /// </devdoc>
        public abstract IList Evaluate(XmlReader contextDocument, XmlResolver dataSources, XsltArgumentList argumentList);
    }
}
