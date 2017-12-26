//------------------------------------------------------------------------------
// <copyright file="XmlILTrace.cs" company="Microsoft">
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
using System;
using System.IO;
using System.Security;
using System.Xml;
using System.Globalization;
using System.Xml.Xsl.Qil;

namespace System.Xml.Xsl.IlGen {

    /// <summary>
    /// Helper class that facilitates tracing of ILGen.
    /// </summary>
    internal static class XmlILTrace {
        private const int MAX_REWRITES = 200;

        /// <summary>
        /// Check environment variable in order to determine whether to write out trace files.  This really should be a
        /// check of the configuration file, but System.Xml does not yet have a good tracing story.
        /// </summary>
        private static string dirName = null;
        private static bool alreadyCheckedEnabled = false;

        /// <summary>
        /// True if tracing has been enabled (environment variable set).
        /// </summary>
        public static bool IsEnabled {
            get {
                // If environment variable has not yet been checked, do so now
                if (!alreadyCheckedEnabled) {
                    try {
                        dirName = Environment.GetEnvironmentVariable("XmlILTrace");
                    }
                    catch (SecurityException) {
                        // If user does not have access to environment variables, tracing will remain disabled
                    }

                    alreadyCheckedEnabled = true;
                }

                return (dirName != null);
            }
        }

        /// <summary>
        /// If tracing is enabled, this method will delete the contents of "filename" in preparation for append
        /// operations.
        /// </summary>
        public static void PrepareTraceWriter(string fileName) {
            if (!IsEnabled)
                return;

            File.Delete(dirName + "\\" + fileName);
        }

        /// <summary>
        /// If tracing is enabled, this method will open a TextWriter over "fileName" and return it.  Otherwise,
        /// null will be returned.
        /// </summary>
        public static TextWriter GetTraceWriter(string fileName) {
            if (!IsEnabled)
                return null;

            return new StreamWriter(dirName + "\\" + fileName, true);
        }

        /// <summary>
        /// Serialize Qil tree to "fileName", in the directory identified by "dirName".
        /// </summary>
        public static void WriteQil(QilExpression qil, string fileName) {
            if (!IsEnabled)
                return;

            XmlWriter w = XmlWriter.Create(dirName + "\\" + fileName);
            try {
                WriteQil(qil, w);
            }
            finally {
                w.Close();
            }
        }

        /// <summary>
        /// Trace ILGen optimizations and log them to "fileName".
        /// </summary>
        public static void TraceOptimizations(QilExpression qil, string fileName) {
            if (!IsEnabled)
                return;

            XmlWriter w = XmlWriter.Create(dirName + "\\" + fileName);

            w.WriteStartDocument();
            w.WriteProcessingInstruction("xml-stylesheet", "href='qilo.xslt' type='text/xsl'");
            w.WriteStartElement("QilOptimizer");
            w.WriteAttributeString("timestamp", DateTime.Now.ToString(CultureInfo.InvariantCulture));
            WriteQilRewrite(qil, w, null);

            try {
                // Then, rewrite the graph until "done" or some max value is reached.
                for (int i = 1; i < MAX_REWRITES; i++) {
                    QilExpression qilTemp = (QilExpression) (new QilCloneVisitor(qil.Factory).Clone(qil));

                    XmlILOptimizerVisitor visitor = new XmlILOptimizerVisitor(qilTemp, !qilTemp.IsDebug);
                    visitor.Threshold = i;
                    qilTemp = visitor.Optimize();

                    // In debug code, ensure that QIL after N steps is correct
                    QilValidationVisitor.Validate(qilTemp);

                    // Trace the rewrite
                    WriteQilRewrite(qilTemp, w, OptimizationToString(visitor.LastReplacement));

                    if (visitor.ReplacementCount < i)
                        break;
                }
            }
            catch (Exception e) {
                if (!XmlException.IsCatchableException(e)) {
                    throw;
                }
                w.WriteElementString("Exception", null, e.ToString());
                throw;
            }
            finally {
                w.WriteEndElement();
                w.WriteEndDocument();
                w.Flush();
                w.Close();
            }
        }

        /// <summary>
        /// Serialize Qil tree to writer "w".
        /// </summary>
        private static void WriteQil(QilExpression qil, XmlWriter w) {
            QilXmlWriter qw = new QilXmlWriter(w);
            qw.ToXml(qil);
        }

        /// <summary>
        /// Serialize rewritten Qil tree to writer "w".
        /// </summary>
        private static void WriteQilRewrite(QilExpression qil, XmlWriter w, string rewriteName) {
            w.WriteStartElement("Diff");
            if (rewriteName != null)
                w.WriteAttributeString("rewrite", rewriteName);
            WriteQil(qil, w);
            w.WriteEndElement();
        }

        /// <summary>
        /// Get friendly string description of an ILGen optimization.
        /// </summary>
        private static string OptimizationToString(int opt) {
            string s = Enum.GetName(typeof(XmlILOptimization), opt);
            if (s.StartsWith("Introduce", StringComparison.Ordinal)) {
                return s.Substring(9) + " introduction";
            }
            else if (s.StartsWith("Eliminate", StringComparison.Ordinal)) {
                return s.Substring(9) + " elimination";
            }
            else if (s.StartsWith("Commute", StringComparison.Ordinal)) {
                return s.Substring(7) + " commutation";
            }
            else if (s.StartsWith("Fold", StringComparison.Ordinal)) {
                return s.Substring(4) + " folding";
            }
            else if (s.StartsWith("Misc", StringComparison.Ordinal)) {
                return s.Substring(4);
            }
            return s;
        }
    }
}
