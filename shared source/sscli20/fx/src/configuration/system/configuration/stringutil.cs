//------------------------------------------------------------------------------
// <copyright file="StringUtil.cs" company="Microsoft">
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

namespace System.Configuration {
    using System.Collections;
    using System.Collections.Specialized;
    using System.Configuration;
    using System.Globalization;
    using System.IO;
    using System.Runtime.InteropServices;
    using System.Security.Permissions;
    using System.Security;
    using System.Text;
    using System.Xml;
    using Microsoft.Win32;

    static internal class StringUtil {


        /*
         * Determines if two strings are equal. Treats null and String.Empty as equivalent.
         */
        internal static bool EqualsNE(string s1, string s2) {
            if (s1 == null) {
                s1 = String.Empty;
            }

            if (s2 == null) {
                s2 = String.Empty;
            }

            return String.Equals(s1, s2, StringComparison.Ordinal);
        }

        /*
         * Determines if two strings are equal, ignoring case.
         */
        internal static bool EqualsIgnoreCase(string s1, string s2) {
            return String.Equals(s1, s2, StringComparison.OrdinalIgnoreCase);
        }


        /*
         * Determines if the first string starts with the second string, ignoring case.
         */
        internal static bool StartsWith(string s1, string s2) {
            if (s2 == null) {
                return false;
            }

            return 0 == String.Compare(s1, 0, s2, 0, s2.Length, StringComparison.Ordinal);
        }

        /*
         * Determines if the first string starts with the second string, ignoring case.
         */
        internal static bool StartsWithIgnoreCase(string s1, string s2) {
            if (s2 == null) {
                return false;
            }

            return 0 == String.Compare(s1, 0, s2, 0, s2.Length, StringComparison.OrdinalIgnoreCase);
        }
        
        internal static string[] ObjectArrayToStringArray(object[] objectArray) {
            String[] stringKeys = new String[objectArray.Length];
            objectArray.CopyTo(stringKeys, 0);
            return stringKeys;
        }

    }
}
