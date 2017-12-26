//------------------------------------------------------------------------------
// <copyright file="XmlCollation.cs" company="Microsoft">
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
using System.Diagnostics;
using System.Globalization;
using System.Runtime.InteropServices;
using System.ComponentModel;

namespace System.Xml.Xsl.Runtime {
    using Res = System.Xml.Utils.Res;

    [EditorBrowsable(EditorBrowsableState.Never)]
    public sealed class XmlCollation {
        // lgid support for sort
        private const int deDE = 0x0407;
        private const int huHU = 0x040E;
        private const int jaJP = 0x0411;
        private const int kaGE = 0x0437;
        private const int koKR = 0x0412;
        private const int zhTW = 0x0404;
        private const int zhCN = 0x0804;
        private const int zhHK = 0x0C04;
        private const int zhSG = 0x1004;
        private const int zhMO = 0x1404;
        private const int zhTWbopo = 0x030404;
        private const int deDEphon = 0x010407;
        private const int huHUtech = 0x01040e;
        private const int kaGEmode = 0x010437;

        // Sort ID
        private const int strksort = 0x02;      // Stroke
        private const int unicsort = 0x01;      // Unicode

        // Options
        private const string ignoreCaseStr      = "IGNORECASE";
        private const string ignoreKanatypeStr  = "IGNOREKANATYPE";
        private const string ignoreNonspaceStr  = "IGNORENONSPACE";
        private const string ignoreSymbolsStr   = "IGNORESYMBOLS";
        private const string ignoreWidthStr     = "IGNOREWIDTH";
        private const string upperFirstStr      = "UPPERFIRST";
        private const string emptyGreatestStr   = "EMPTYGREATEST";
        private const string descendingOrderStr = "DESCENDINGORDER";
        private const string sortStr            = "SORT";

        private bool upperFirst;
        private bool emptyGreatest;
        private bool descendingOrder;
        private CultureInfo cultinfo;
        private CompareOptions compops;


        //-----------------------------------------------
        // Constructors
        //-----------------------------------------------

        /// <summary>
        /// By default, create a collation that uses the current thread's culture, and has no compare options set
        /// </summary>
        private XmlCollation() : this(null, CompareOptions.None) {
        }

        /// <summary>
        /// Construct a collation that uses the specified culture and compare options.
        /// </summary>
        private XmlCollation(CultureInfo cultureInfo, CompareOptions compareOptions) {
            this.cultinfo = cultureInfo;
            this.compops = compareOptions;
        }


        //-----------------------------------------------
        // Create
        //-----------------------------------------------

        /// <summary>
        /// Singleton collation that sorts according to Unicode code points.
        /// </summary>
        private static XmlCollation cp = new XmlCollation(CultureInfo.InvariantCulture, CompareOptions.Ordinal);
        internal static XmlCollation CodePointCollation {
            get { return cp; }
        }

        /// <summary>
        /// This function is used in both parser and f&o library, so just strictly map valid literals to XmlCollation.
        /// Set compare options one by one:
        ///     0, false: no effect; 1, true: yes
        /// Disregard unrecognized options.
        /// </summary>
        internal static XmlCollation Create(string collationLiteral) {
            Debug.Assert(collationLiteral != null, "collation literal should not be null");

            if (collationLiteral == XmlReservedNs.NsCollCodePoint) {
                return CodePointCollation;
            }

            XmlCollation coll = new XmlCollation();
            Uri collationUri = new Uri(collationLiteral);

            string authority = collationUri.GetLeftPart(UriPartial.Authority);
            if (authority == XmlReservedNs.NsCollationBase) {
                // Language
                // at least a '/' will be returned for Uri.LocalPath
                string lang = collationUri.LocalPath.Substring(1);
                if (lang.Length == 0) {
                    // Use default culture of current thread (cultinfo = null)
                } else {
                    // Create culture from RFC 1766 string
                    try {
                        coll.cultinfo = new CultureInfo(lang);
                    }
                    catch (ArgumentException) {
                        throw new XslTransformException(Res.Coll_UnsupportedLanguage, lang);
                    }
                }
            } else if (collationUri.IsBaseOf(new Uri(XmlReservedNs.NsCollCodePoint))) {
                // language with codepoint collation is not allowed
                coll.compops = CompareOptions.Ordinal;
            } else {
                // Unrecognized collation
                throw new XslTransformException(Res.Coll_Unsupported, collationLiteral);
            }

            // Sort & Compare option
            // at least a '?' will be returned for Uri.Query if not empty
            string query = collationUri.Query;
            string sort = null;

            if (query.Length != 0) {
                foreach (string option in query.Substring(1).Split('&')) {
                    string[] pair = option.Split('=');

                    if (pair.Length != 2)
                        throw new XslTransformException(Res.Coll_BadOptFormat, option);

                    string optionName = pair[0].ToUpper(CultureInfo.InvariantCulture);
                    string optionValue = pair[1].ToUpper(CultureInfo.InvariantCulture);

                    if (optionName == sortStr) {
                        sort = optionValue;
                    }
                    else if (optionValue == "1" || optionValue == "TRUE") {
                        switch (optionName) {
                        case ignoreCaseStr:        coll.compops |= CompareOptions.IgnoreCase; break;
                        case ignoreKanatypeStr:    coll.compops |= CompareOptions.IgnoreKanaType; break;
                        case ignoreNonspaceStr:    coll.compops |= CompareOptions.IgnoreNonSpace; break;
                        case ignoreSymbolsStr:     coll.compops |= CompareOptions.IgnoreSymbols; break;
                        case ignoreWidthStr:       coll.compops |= CompareOptions.IgnoreWidth; break;
                        case upperFirstStr:        coll.upperFirst = true; break;
                        case emptyGreatestStr:     coll.emptyGreatest = true; break;
                        case descendingOrderStr:   coll.descendingOrder = true; break;
                        default:
                            throw new XslTransformException(Res.Coll_UnsupportedOpt, pair[0]);
                        }
                    }
                    else if (optionValue == "0" || optionValue == "FALSE") {
                        switch (optionName) {
                        case ignoreCaseStr:        coll.compops &= ~CompareOptions.IgnoreCase; break;
                        case ignoreKanatypeStr:    coll.compops &= ~CompareOptions.IgnoreKanaType; break;
                        case ignoreNonspaceStr:    coll.compops &= ~CompareOptions.IgnoreNonSpace; break;
                        case ignoreSymbolsStr:     coll.compops &= ~CompareOptions.IgnoreSymbols; break;
                        case ignoreWidthStr:       coll.compops &= ~CompareOptions.IgnoreWidth; break;
                        case upperFirstStr:        coll.upperFirst = false; break;
                        case emptyGreatestStr:     coll.emptyGreatest = false; break;
                        case descendingOrderStr:   coll.descendingOrder = false; break;
                        default:
                            throw new XslTransformException(Res.Coll_UnsupportedOpt, pair[0]);
                        }
                    }
                    else {
                        throw new XslTransformException(Res.Coll_UnsupportedOptVal, pair[0], pair[1]);
                    }
                }
            }

            // upperfirst option is only meaningful when not ignore case
            if (coll.upperFirst && (coll.compops & CompareOptions.IgnoreCase) != 0)
                coll.upperFirst = false;

            // other CompareOptions are only meaningful if Ordinal comparison is not being used
            if ((coll.compops & CompareOptions.Ordinal) != 0) {
                coll.compops = CompareOptions.Ordinal;
                coll.upperFirst = false;
            }

            // new cultureinfo based on alternate sorting option
            if (sort != null && coll.cultinfo != null) {
                int lgid = GetLangID(coll.cultinfo.LCID);
                switch (sort) {
                case "bopo":
                    if (lgid == zhTW) {
                        coll.cultinfo = new CultureInfo(zhTWbopo);
                    }
                    break;
                case "strk":
                    if (lgid == zhCN || lgid == zhHK || lgid == zhSG || lgid == zhMO) {
                        coll.cultinfo = new CultureInfo(MakeLCID(coll.cultinfo.LCID, strksort));
                    }
                    break;
                case "uni":
                    if (lgid == jaJP || lgid == koKR) {
                        coll.cultinfo = new CultureInfo(MakeLCID(coll.cultinfo.LCID, unicsort));
                    }
                    break;
                case "phn":
                    if (lgid == deDE) {
                        coll.cultinfo = new CultureInfo(deDEphon);
                    }
                    break;
                case "tech":
                    if (lgid == huHU) {
                        coll.cultinfo = new CultureInfo(huHUtech);
                    }
                    break;
                case "mod":
                    // ka-GE(Georgian - Georgia) Modern Sort: 0x00010437
                    if (lgid == kaGE) {
                        coll.cultinfo = new CultureInfo(kaGEmode);
                    }
                    break;
                case "pron": case "dict": case "trad":
                    // es-ES(Spanish - Spain) Traditional: 0x0000040A
                    // They are removing 0x040a (Spanish Traditional sort) in NLS+.
                    // So if you create 0x040a, it's just like 0x0c0a (Spanish International sort).
                    // Thus I don't handle it differently.
                    break;
                default:
                    throw new XslTransformException(Res.Coll_UnsupportedSortOpt, sort);
                }
            }
            return coll;
        }


        //-----------------------------------------------
        // Compare Properties
        //-----------------------------------------------

        internal bool EmptyGreatest {
            get { return this.emptyGreatest; }
        }

        internal bool DescendingOrder {
            get { return this.descendingOrder; }
        }

        internal CultureInfo Culture {
            get {
                // Use default thread culture if this.cultinfo = null
                if (this.cultinfo == null)
                    return CultureInfo.CurrentCulture;

                return this.cultinfo;
            }
        }


        //-----------------------------------------------
        //
        //-----------------------------------------------

        /// <summary>
        /// Create a sort key that can be compared quickly with other keys.
        /// </summary>
        internal XmlSortKey CreateSortKey(string s) {
            SortKey sortKey;
            byte[] bytesKey;
            int idx;

            sortKey = Culture.CompareInfo.GetSortKey(s, this.compops);

            // Create an XmlStringSortKey using the SortKey if possible
        #if DEBUG
            // In debug-only code, test other code path more frequently
            if (!this.upperFirst && this.descendingOrder)
                return new XmlStringSortKey(sortKey, this.descendingOrder);
        #else
            if (!this.upperFirst)
                return new XmlStringSortKey(sortKey, this.descendingOrder);
        #endif

            // Get byte buffer from SortKey and modify it
            bytesKey = sortKey.KeyData;
            if (this.upperFirst && bytesKey.Length != 0) {
                // By default lower-case is always sorted first for any locale (verified by empirical testing).
                // In order to place upper-case first, invert the case weights in the generated sort key.
                // Skip to case weight section (3rd weight section)
                idx = 0;
                while (bytesKey[idx] != 1)
                    idx++;

                do {
                    idx++;
                }
                while (bytesKey[idx] != 1);

                // Invert all case weights (including terminating 0x1)
                do {
                    idx++;
                    bytesKey[idx] ^= 0xff;
                }
                while (bytesKey[idx] != 0xfe);
            }

            return new XmlStringSortKey(bytesKey, this.descendingOrder);
        }



        //-----------------------------------------------
        // Helper Functions
        //-----------------------------------------------

        private static int MakeLCID(int langid, int sortid) {
            return (langid & 0xffff) | ((sortid & 0xf) << 16);
        }

        private static int GetLangID(int lcid) {
            return (lcid & 0xffff);
        }
    }
}
