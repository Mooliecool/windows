//------------------------------------------------------------------------------
// <copyright file="_IPv4Address.cs" company="Microsoft">
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

namespace System {

    // The class designed as to keep minimal the working set of Uri class.
    // The idea is to stay with static helper methods and strings
    internal class IPv4AddressHelper {

    // fields
        private IPv4AddressHelper() {
        }

        private const int NumberOfLabels = 4;

        // methods

        internal static string ParseCanonicalName(string str, int start, int end, ref bool isLoopback) {
            unsafe {
                byte* numbers = stackalloc byte[NumberOfLabels];
                isLoopback = Parse(str, numbers, start, end);
                return numbers[0] + "." + numbers[1] + "." + numbers[2] + "." + numbers[3];
            }
        }

        internal static int ParseHostNumber(string str, int start, int end) {
            unsafe {
                byte* numbers = stackalloc byte[NumberOfLabels];
                Parse(str, numbers, start, end);
                return (numbers[0] << 24) + (numbers[1] << 16) + (numbers[2] << 8) + numbers[3];
            }
        }

        //
        // IsValid
        //
        //  Performs IsValid on a substring. Updates the index to where we
        //  believe the IPv4 address ends
        //
        // Inputs:
        //  <argument>  name
        //      string containing possible IPv4 address
        //
        //  <argument>  start
        //      offset in <name> to start checking for IPv4 address
        //
        //  <argument>  end
        //      offset in <name> of the last character we can touch in the check
        //
        // Outputs:
        //  <argument>  end
        //      index of last character in <name> we checked
        //
        // Assumes:
        // The address string is terminated by either
        // end of the string, characters ':' '/' '\' '?'
        //
        //
        // Returns:
        //  bool
        //
        // Throws:
        //  Nothing
        //

        //internal unsafe static bool IsValid(char* name, int start, ref int end) {
        //    return IsValid(name, start, ref end, false);
        //}

        //Remark: MUST NOT be used unless all input indexes are are verified and trusted.
        internal unsafe static bool IsValid(char* name, int start, ref int end, bool allowIPv6, bool notImplicitFile) {

            int dots = 0;
            int number = 0;
            bool haveNumber = false;

            while (start < end) {
                char ch = name[start];
                if (allowIPv6) {
                    // for ipv4 inside ipv6 the terminator is either ScopeId, prefix or ipv6 terminator
                    if(ch == ']' || ch == '/' || ch == '%') break;
                }
                else if (ch == '/' || ch == '\\' || (notImplicitFile && (ch == ':' || ch == '?' || ch == '#'))) {
                    break;
                }

                if (ch <= '9' && ch >= '0') {
                    haveNumber = true;
                    number = number * 10 + (name[start] - '0');
                    if (number > 255) {
                        return false;
                    }
                } else if (ch == '.') {
                    if (!haveNumber) {
                        return false;
                    }
                    ++dots;
                    haveNumber = false;
                    number = 0;
                } else {
                    return false;
                }
                ++start;
            }
            bool res = (dots == 3) && haveNumber;
            if (res) {
                end = start;
            }
            return res;
        }

        //
        // Parse
        //
        //  Convert this IPv4 address into a sequence of 4 8-bit numbers
        //
        // Assumes:
        //  <Name> has been validated and contains only decimal digits in groups
        //  of 8-bit numbers and the characters '.'
        //  Address may terminate with ':' or with the end of the string
        //
        unsafe private static bool Parse(string name, byte* numbers, int start, int end) {
            for (int i = 0; i < NumberOfLabels; ++i) {

                byte b = 0;
                char ch;
                for (; (start < end) && (ch = name[start]) != '.' && ch != ':'; ++start) {
                    b = (byte)(b * 10 + (byte)(ch - '0'));
                }
                numbers[i] = b;
                ++start;
            }
            return numbers[0] == 127;
        }
    }
}
