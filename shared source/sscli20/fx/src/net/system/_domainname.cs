//------------------------------------------------------------------------------
// <copyright file="_DomainName.cs" company="Microsoft">
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

using System.Globalization;

namespace System {

    // The class designed as to keep working set of Uri class as minimal.
    // The idea is to stay with static helper methods and strings
    internal class DomainNameHelper {

        private DomainNameHelper() {
        }

        internal const string Localhost = "localhost";
        internal const string Loopback = "loopback";

        internal static string ParseCanonicalName(string str,int start, int end, ref bool loopback) {
            string res = null;

            for (int i = end-1; i >= start; --i) {
                if (str[i] >= 'A' && str[i] <= 'Z') {
                    res = str.Substring(start, end-start).ToLower(CultureInfo.InvariantCulture);
                    break;
                }
                if (str[i] == ':')
                    end = i;
            }

            if (res == null) {
                res = str.Substring(start, end-start);
            }

            if (res == Localhost || res == Loopback) {
                loopback = true;
                return Localhost;
            }
            return res;
        }
        //
        // IsValid
        //
        //  Determines whether a string is a valid domain name
        //
        //      subdomain -> <label> | <label> "." <subdomain>
        //
        // Inputs:
        //  - name as Name to test
        //  - starting position
        //  - ending position
        //
        // Outputs:
        //  The end position of a valid domain name string, the canonical flag if found so
        //
        // Returns:
        //  bool
        //
        //  Remarks: Optimized for speed as a most comon case,
        //           MUST NOT be used unless all input indexes are are verified and trusted.
        //

        internal unsafe static bool IsValid(char* name, ushort pos, ref int returnedEnd, ref bool notCanonical, bool notImplicitFile) {

            char *curPos = name + pos;
            char *newPos = curPos;
            char *end    = name + returnedEnd;
            for (; newPos < end; ++newPos) {
                char ch = *newPos;
                if (ch == '/' || ch == '\\' || (notImplicitFile && (ch == ':' || ch == '?' || ch == '#'))) {
                    end = newPos;
                    break;
                }
            }

            if (end == curPos) {
                return false;
            }

            do {
                //  Determines whether a string is a valid domain name label. In keeping
                //  with RFC 1123, section 2.1, the requirement that the first character
                //  of a label be alphabetic is dropped. Therefore, Domain names are
                //  formed as:
                //
                //      <label> -> <alphanum> [<alphanum> | <hyphen> | <underscore>] * 62

                //find the dot or hit the end
                newPos = curPos;
                while (newPos < end) {
                    if (*newPos == '.') break;
                    ++newPos;
                }

                //check the label start/range
                if (curPos == newPos || newPos-curPos > 63 || !IsASCIILetterOrDigit(*curPos++, ref notCanonical)) {
                    return false;
                }
                //check the label content
                while(curPos < newPos) {
                    if (!IsValidDomainLabelCharacter(*curPos++, ref notCanonical)) {
                        return false;
                    }
                }
                ++curPos;

            } while (curPos < end);

            returnedEnd = (ushort)(end-name);
            return true;
        }
        //  Determines whether a character is a valid letter according to
        //  RFC 1035. Note: we don't use Char.IsLetter() because it assumes
        //  some non-ANSI characters out of the range A..Za..z are also
        //  valid letters
        //
        private static bool IsASCIILetter(char character, ref bool notCanonical) {

            if (character >= 'a' && character <= 'z')   return true;

            if (character >= 'A' && character <= 'Z')
            {
                if (!notCanonical) notCanonical = true;
                return true;
            }
            return false;
        }
        //
        //  Determines whether a character is a letter or digit according to the
        //  DNS specification [RFC 1035]. We use our own variant of IsLetterOrDigit
        //  because the base version returns false positives for non-ANSI characters
        //
        private static bool IsASCIILetterOrDigit(char character, ref bool notCanonical) {
            return IsASCIILetter(character, ref notCanonical) || (character >= '0' && character <= '9');
        }
        //
        //  Takes into account the additional legal domain name characters '-' and '_'
        //  Note that '_' char is formally invalid but is historically in use, especially on corpnets
        //
        private static bool IsValidDomainLabelCharacter(char character, ref bool notCanonical) {
            return IsASCIILetterOrDigit(character, ref notCanonical) || (character == '-') || (character == '_');
        }

    }
}
