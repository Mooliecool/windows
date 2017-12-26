/********************************** Module Header **********************************\
* Module Name:  HexStringByteArrayConverter.cs
* Project:      CSHexStringByteArrayConverter
* Copyright (c) Microsoft Corporation.
*
* The class provides helper functions of converting hex string to byte array, 
* converting byte array to hex string, and verifying the hex string input. 
*
* This source is subject to the Microsoft Public License.
* See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
* All other rights reserved.
*
* THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER 
* EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF 
* MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
\***********************************************************************************/

using System;
using System.Text;
using System.Text.RegularExpressions;


namespace CSHexStringByteArrayConverter
{
    class HexStringByteArrayConverter
    {
        private const string hexDigits = "0123456789ABCDEF";

        /// <summary>
        /// Convert a byte array to hex string. Example output: "7F2C4A00".
        /// 
        /// Alternatively, you can also use the BitConverter.ToString method to 
        /// convert byte array to string of hexadecimal pairs separated by hyphens, 
        /// where each pair represents the corresponding element in value; for 
        /// example, "7F-2C-4A-00".
        /// </summary>
        /// <param name="bytes">An array of bytes</param>
        /// <returns>Hex string</returns>
        public static string BytesToHexString(byte[] bytes)
        {
            StringBuilder sb = new StringBuilder(bytes.Length * 2);
            foreach (byte b in bytes)
            {
                sb.AppendFormat("{0:X2}", b);
            }
            return sb.ToString();
        }

        /// <summary>
        /// Convert a hex string to byte array.
        /// </summary>
        /// <param name="str">hex string. For example, "FF00EE11"</param>
        /// <returns>An array of bytes</returns>
        public static byte[] HexStringToBytes(string str)
        {
            // Determine the number of bytes
            byte[] bytes = new byte[str.Length >> 1];
            for (int i = 0; i < str.Length; i += 2)
            {
                int highDigit = hexDigits.IndexOf(Char.ToUpperInvariant(str[i]));
                int lowDigit = hexDigits.IndexOf(Char.ToUpperInvariant(str[i + 1]));
                if (highDigit == -1 || lowDigit == -1)
                {
                    throw new ArgumentException("The string contains an invalid digit.", "s");
                }
                bytes[i >> 1] = (byte)((highDigit << 4) | lowDigit);
            }
            return bytes;
        }

        /// <summary>
        /// Verify the format of the hex string.
        /// </summary>
        public static bool VerifyHexString(string str)
        {
            Regex regex = new Regex("\\A[0-9a-fA-F]+\\z");
            return regex.IsMatch(str) && ((str.Length & 1) != 1);
        }
    }
}