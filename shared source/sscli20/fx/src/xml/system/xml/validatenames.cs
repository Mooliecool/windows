//------------------------------------------------------------------------------
// <copyright file="ValidateNames.cs" company="Microsoft">
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
using System.Xml.XPath;
using System.Diagnostics;

namespace System.Xml {

    /// <summary>
    /// Contains various static functions and methods for parsing and validating:
    ///     NCName (not namespace-aware, no colons allowed)
    ///     QName (prefix:local-name)
    /// </summary>
    internal class ValidateNames {
        // Not creatable
        private ValidateNames() {
        }

        public enum Flags {
            NCNames = 0x1,              // Validate that each non-empty prefix and localName is a valid NCName
            CheckLocalName = 0x2,       // Validate the local-name
            CheckPrefixMapping = 0x4,   // Validate the prefix --> namespace mapping
            All = 0x7,
            AllExceptNCNames = 0x6,
            AllExceptPrefixMapping = 0x3,
        };


        //-----------------------------------------------
        // NCName parsing
        //-----------------------------------------------

        /// <summary>
        /// Attempts to parse the input string as an NCName (see the XML Namespace spec).
        /// Quits parsing when an invalid NCName char is reached or the end of string is reached.
        /// Returns the number of valid NCName chars that were parsed.
        /// </summary>
        public static unsafe int ParseNCName(string s, int offset) {
            int offsetStart = offset;
            XmlCharType xmlCharType = XmlCharType.Instance;
            Debug.Assert(s != null && offset <= s.Length);

            // Quit if the first character is not a valid NCName starting character
            if (offset < s.Length && 
                (xmlCharType.charProperties[s[offset]] & XmlCharType.fNCStartName) != 0) { // xmlCharType.IsStartNCNameChar(s[offset])) {

                // Keep parsing until the end of string or an invalid NCName character is reached
                for (offset++; offset < s.Length; offset++) {
                    if ((xmlCharType.charProperties[s[offset]] & XmlCharType.fNCName) == 0) // if (!xmlCharType.IsNCNameChar(s[offset]))
                        break;
                }
            }

            return offset - offsetStart;
        }

        /// <summary>
        /// Calls parseName and throws exception if the resulting name is not a valid NCName.
        /// Returns the input string if there is no error.
        /// </summary>
        public static string ParseNCNameThrow(string s) {
            // throwOnError = true
            ParseNCNameInternal(s, true);
            return s;
        }

        /// <summary>
        /// Calls parseName and returns false or throws exception if the resulting name is not
        /// a valid NCName.  Returns the input string if there is no error.
        /// </summary>
        private static bool ParseNCNameInternal(string s, bool throwOnError) {
            int len = ParseNCName(s, 0);

            if (len == 0 || len != s.Length) {
                // If the string is not a valid NCName, then throw or return false
                if (throwOnError) ThrowInvalidName(s, 0, len);
                return false;
            }

            return true;
        }


        //-----------------------------------------------
        // QName parsing
        //-----------------------------------------------

        /// <summary>
        /// Attempts to parse the input string as a QName (see the XML Namespace spec).
        /// Quits parsing when an invalid QName char is reached or the end of string is reached.
        /// Returns the number of valid QName chars that were parsed.
        /// Sets colonOffset to the offset of a colon character if it exists, or 0 otherwise.
        /// </summary>
        public static int ParseQName(string s, int offset, out int colonOffset) {
            int len, lenLocal;

            // Assume no colon
            colonOffset = 0;

            // Parse NCName (may be prefix, may be local name)
            len = ParseNCName(s, offset);
            if (len != 0) {

                // Non-empty NCName, so look for colon if there are any characters left
                offset += len;
                if (offset < s.Length && s[offset] == ':') {

                    // First NCName was prefix, so look for local name part
                    lenLocal = ParseNCName(s, offset + 1);
                    if (lenLocal != 0) {
                        // Local name part found, so increase total QName length (add 1 for colon)
                        colonOffset = offset;
                        len += lenLocal + 1;
                    }
                }
            }

            return len;
        }

        /// <summary>
        /// Calls parseQName and throws exception if the resulting name is not a valid QName.
        /// Returns the prefix and local name parts.
        /// </summary>
        public static void ParseQNameThrow(string s, out string prefix, out string localName) {
            int colonOffset;
            int len = ParseQName(s, 0, out colonOffset);

            if (len == 0 || len != s.Length) {
                // If the string is not a valid QName, then throw
                ThrowInvalidName(s, 0, len);
            }

            if (colonOffset != 0) {
                prefix = s.Substring(0, colonOffset);
                localName = s.Substring(colonOffset + 1);
            }
            else {
                prefix = "";
                localName = s;
            }
        }

        /// <summary>
        /// Parses the input string as a NameTest (see the XPath spec), returning the prefix and
        /// local name parts.  Throws an exception if the given string is not a valid NameTest.
        /// If the NameTest contains a star, null values for localName (case NCName':*'), or for
        /// both localName and prefix (case '*') are returned.
        /// </summary>
        public static void ParseNameTestThrow(string s, out string prefix, out string localName) {
            int len, lenLocal, offset;

            if (s.Length != 0 && s[0] == '*') {
                // '*' as a NameTest
                prefix = localName = null;
                len = 1;
            }
            else {
                // Parse NCName (may be prefix, may be local name)
                len = ParseNCName(s, 0);
                if (len != 0) {

                    // Non-empty NCName, so look for colon if there are any characters left
                    localName = s.Substring(0, len);
                    if (len < s.Length && s[len] == ':') {

                        // First NCName was prefix, so look for local name part
                        prefix = localName;
                        offset = len + 1;
                        if (offset < s.Length && s[offset] == '*') {
                            // '*' as a local name part, add 2 to len for colon and star
                            localName = null;
                            len += 2;
                        }
                        else {
                            lenLocal = ParseNCName(s, offset);
                            if (lenLocal != 0) {
                                // Local name part found, so increase total NameTest length
                                localName = s.Substring(offset, lenLocal);
                                len += lenLocal + 1;
                            }
                        }
                    }
                    else {
                        prefix = string.Empty;
                    }
                }
                else {
                    // Make the compiler happy
                    prefix = localName = null;
                }
            }

            if (len == 0 || len != s.Length) {
                // If the string is not a valid NameTest, then throw
                ThrowInvalidName(s, 0, len);
            }
        }

        /// <summary>
        /// Throws an invalid name exception.
        /// </summary>
        /// <param name="s">String that was parsed.</param>
        /// <param name="offsetStartChar">Offset in string where parsing began.</param>
        /// <param name="offsetBadChar">Offset in string where parsing failed.</param>
        public static void ThrowInvalidName(string s, int offsetStartChar, int offsetBadChar) {
            // If the name is empty, throw an exception
            if (offsetStartChar >= s.Length)
                throw new XmlException(Res.Xml_EmptyName, string.Empty);

            Debug.Assert(offsetBadChar < s.Length);

            if (XmlCharType.Instance.IsNCNameChar(s[offsetBadChar]) && !XmlCharType.Instance.IsStartNCNameChar(s[offsetBadChar])) {
                // The error character is a valid name character, but is not a valid start name character
                throw new XmlException(Res.Xml_BadStartNameChar, XmlException.BuildCharExceptionStr(s[offsetBadChar]));
            }
            else {
                // The error character is an invalid name character
                throw new XmlException(Res.Xml_BadNameChar, XmlException.BuildCharExceptionStr(s[offsetBadChar]));
            }
        }

        public static Exception GetInvalidNameException(string s, int offsetStartChar, int offsetBadChar) {
            // If the name is empty, throw an exception
            if (offsetStartChar >= s.Length)
                return new XmlException(Res.Xml_EmptyName, string.Empty);

            Debug.Assert(offsetBadChar < s.Length);

            if (XmlCharType.Instance.IsNCNameChar(s[offsetBadChar]) && !XmlCharType.Instance.IsStartNCNameChar(s[offsetBadChar])) {
                // The error character is a valid name character, but is not a valid start name character
                return new XmlException(Res.Xml_BadStartNameChar, XmlException.BuildCharExceptionStr(s[offsetBadChar]));
            }
            else {
                // The error character is an invalid name character
                return new XmlException(Res.Xml_BadNameChar, XmlException.BuildCharExceptionStr(s[offsetBadChar]));
            }
        }
        /// <summary>
        /// Returns true if "prefix" starts with the characters 'x', 'm', 'l' (case-insensitive).
        /// </summary>
        public static bool StartsWithXml(string s) {
            if (s.Length < 3)
                return false;

            if (s[0] != 'x' && s[0] != 'X')
                return false;

            if (s[1] != 'm' && s[1] != 'M')
                return false;

            if (s[2] != 'l' && s[2] != 'L')
                return false;

            return true;
        }

        /// <summary>
        /// Returns true if "s" is a namespace that is reserved by Xml 1.0 or Namespace 1.0.
        /// </summary>
        public static bool IsReservedNamespace(string s) {
            return s.Equals(XmlReservedNs.NsXml) || s.Equals(XmlReservedNs.NsXmlNs);
        }

        /// <summary>
        /// Throw if the specified name parts are not valid according to the rules of "nodeKind".  Check only rules that are
        /// specified by the Flags.
        /// NOTE: Namespaces should be passed using a prefix, ns pair.  "localName" is always string.Empty.
        /// </summary>
        public static void ValidateNameThrow(string prefix, string localName, string ns, XPathNodeType nodeKind, Flags flags) {
            // throwOnError = true
            ValidateNameInternal(prefix, localName, ns, nodeKind, flags, true);
        }

        /// <summary>
        /// Return false if the specified name parts are not valid according to the rules of "nodeKind".  Check only rules that are
        /// specified by the Flags.
        /// NOTE: Namespaces should be passed using a prefix, ns pair.  "localName" is always string.Empty.
        /// </summary>
        public static bool ValidateName(string prefix, string localName, string ns, XPathNodeType nodeKind, Flags flags) {
            // throwOnError = false
            return ValidateNameInternal(prefix, localName, ns, nodeKind, flags, false);
        }

        /// <summary>
        /// Return false or throw if the specified name parts are not valid according to the rules of "nodeKind".  Check only rules
        /// that are specified by the Flags.
        /// NOTE: Namespaces should be passed using a prefix, ns pair.  "localName" is always string.Empty.
        /// </summary>
        private static bool ValidateNameInternal(string prefix, string localName, string ns, XPathNodeType nodeKind, Flags flags, bool throwOnError) {
            Debug.Assert(prefix != null && localName != null && ns != null);

            if ((flags & Flags.NCNames) != 0) {

                // 1. Verify that each non-empty prefix and localName is a valid NCName
                if (prefix.Length != 0)
                    if (!ParseNCNameInternal(prefix, throwOnError)) {
                        return false;
                    }

                if (localName.Length != 0)
                    if (!ParseNCNameInternal(localName, throwOnError)) {
                        return false;
                    }
            }

            if ((flags & Flags.CheckLocalName) != 0) {

                // 2. Determine whether the local name is valid
                switch (nodeKind) {
                    case XPathNodeType.Element:
                        // Elements and attributes must have a non-empty local name
                        if (localName.Length == 0) {
                            if (throwOnError) throw new XmlException(Res.Xdom_Empty_LocalName, string.Empty);
                            return false;
                        }
                        break;

                    case XPathNodeType.Attribute:
                        // Attribute local name cannot be "xmlns" if namespace is empty
                        if (ns.Length == 0 && localName.Equals("xmlns")) {
                            if (throwOnError) throw new XmlException(Res.XmlBadName, new string[] {nodeKind.ToString(), localName});
                            return false;
                        }
                        goto case XPathNodeType.Element;

                    case XPathNodeType.ProcessingInstruction:
                        // PI's local-name must be non-empty and cannot be 'xml' (case-insensitive)
                        if (localName.Length == 0 || (localName.Length == 3 && StartsWithXml(localName))) {
                            if (throwOnError) throw new XmlException(Res.Xml_InvalidPIName, localName);
                            return false;
                        }
                        break;

                    default:
                        // All other node types must have empty local-name
                        if (localName.Length != 0) {
                            if (throwOnError) throw new XmlException(Res.XmlNoNameAllowed, nodeKind.ToString());
                            return false;
                        }
                        break;
                }
            }

            if ((flags & Flags.CheckPrefixMapping) != 0) {

                // 3. Determine whether the prefix is valid
                switch (nodeKind) {
                    case XPathNodeType.Element:
                    case XPathNodeType.Attribute:
                    case XPathNodeType.Namespace:
                        if (ns.Length == 0) {
                            // If namespace is empty, then prefix must be empty
                            if (prefix.Length != 0) {
                                if (throwOnError) throw new XmlException(Res.Xml_PrefixForEmptyNs, string.Empty);
                                return false;
                            }
                        }
                        else {
                            // Don't allow empty attribute prefix since namespace is non-empty
                            if (prefix.Length == 0 && nodeKind == XPathNodeType.Attribute) {
                                if (throwOnError) throw new XmlException(Res.XmlBadName, new string[] {nodeKind.ToString(), localName});
                                return false;
                            }

                            if (prefix.Equals("xml")) {
                                // xml prefix must be mapped to the xml namespace
                                if (!ns.Equals(XmlReservedNs.NsXml)) {
                                    if (throwOnError) throw new XmlException(Res.Xml_XmlPrefix, string.Empty);
                                    return false;
                                }
                            }
                            else if (prefix.Equals("xmlns")) {
                                // Prefix may never be 'xmlns'
                                if (throwOnError) throw new XmlException(Res.Xml_XmlnsPrefix, string.Empty);
                                return false;
                            }
                            else if (IsReservedNamespace(ns)) {
                                // Don't allow non-reserved prefixes to map to xml or xmlns namespaces
                                if (throwOnError) throw new XmlException(Res.Xml_NamespaceDeclXmlXmlns, string.Empty);
                                return false;
                            }
                        }
                        break;

                    case XPathNodeType.ProcessingInstruction:
                        // PI's prefix and namespace must be empty
                        if (prefix.Length != 0 || ns.Length != 0) {
                            if (throwOnError) throw new XmlException(Res.Xml_InvalidPIName, CreateName(prefix, localName));
                            return false;
                        }
                        break;

                    default:
                        // All other node types must have empty prefix and namespace
                        if (prefix.Length != 0 || ns.Length != 0) {
                            if (throwOnError) throw new XmlException(Res.XmlNoNameAllowed, nodeKind.ToString());
                            return false;
                        }
                        break;
                }
            }

            return true;
        }

        /// <summary>
        /// Creates a colon-delimited qname from prefix and local name parts.
        /// </summary>
        private static string CreateName(string prefix, string localName) {
            return (prefix.Length != 0) ? prefix + ":" + localName : localName;
        }

        /// <summary>
        /// Split a QualifiedName into prefix and localname, w/o any checking.
        /// (Used for XmlReader/XPathNavigator MoveTo(name) methods)
        /// </summary>
        internal static void SplitQName(string name, out string prefix, out string lname) {
            int colonPos = name.IndexOf(':');
            if (-1 == colonPos) {
                prefix = string.Empty;
                lname = name;
            }
            else if (0 == colonPos || (name.Length-1) == colonPos) {
                throw new ArgumentException(Res.GetString(Res.Xml_BadNameChar, XmlException.BuildCharExceptionStr(':')), "name");
            }
            else {
                prefix = name.Substring(0, colonPos);
                colonPos++; // move after colon
                lname = name.Substring(colonPos, name.Length - colonPos);
            }
        }
    
    }
}
