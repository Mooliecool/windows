//------------------------------------------------------------------------------
// <copyright file="StringConcat.cs" company="Microsoft">
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
using System.Collections;
using System.Diagnostics;
using System.ComponentModel;

namespace System.Xml.Xsl.Runtime {

    /// <summary>
    /// Efficiently concatenates strings when the number of string is not known beforehand, and
    /// yet it is common for only one string to be concatenated.  StringBuilder is not good for
    /// this purpose, since it *always* allocates objects, even if only one string is appended.
    /// </summary>
    [EditorBrowsable(EditorBrowsableState.Never)]
    public struct StringConcat {
        private string s1, s2, s3, s4;
        private string delimiter;
        private ArrayList strList;
        int idxStr;

        /// <summary>
        /// Clear the result string.
        /// </summary>
        public void Clear() {
            this.idxStr = 0;
            this.delimiter = null;
        }

        /// <summary>
        /// Gets or sets the string that delimits concatenated strings.
        /// </summary>
        public string Delimiter {
            get { return this.delimiter; }
            set { this.delimiter = value; }
        }

        /// <summary>
        /// Concatenate a new string to the result.
        /// </summary>
        public void Concat(string value) {
            Debug.Assert(value != null);

            if (this.delimiter != null && this.idxStr != 0) {
                // Add delimiter
                ConcatNoDelimiter(this.delimiter);
            }

            ConcatNoDelimiter(value);
        }

        /// <summary>
        /// Get the result string.
        /// </summary>
        public string GetResult() {
            switch (this.idxStr) {
                case 0: return string.Empty;
                case 1: return this.s1;
                case 2: return string.Concat(this.s1, this.s2);
                case 3: return string.Concat(this.s1, this.s2, this.s3);
                case 4: return string.Concat(this.s1, this.s2, this.s3, this.s4);
            }
            return string.Concat(this.strList.ToArray());
        }

        /// <summary>
        /// Concatenate a new string to the result without adding a delimiter.
        /// </summary>
        private void ConcatNoDelimiter(string s) {
            switch (this.idxStr) {
                case 0: this.s1 = s; break;
                case 1: this.s2 = s; break;
                case 2: this.s3 = s; break;
                case 3: this.s4 = s; break;
                default:
                    if (this.idxStr == 4) {
                        this.strList = new ArrayList(8);
                        this.strList.Add(this.s1);
                        this.strList.Add(s2);
                        this.strList.Add(s3);
                        this.strList.Add(s4);
                    }
                    this.strList.Add(s);
                    break;
            }

            this.idxStr++;
        }
    }
}
