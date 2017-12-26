//------------------------------------------------------------------------------
// <copyright file="SubstitutionList.cs" company="Microsoft">
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

namespace System.Xml.Xsl.Qil {    

    /// <summary>
    /// Data structure for use in CloneAndReplace
    /// </summary>
    /// <remarks>Isolates the many QilNode classes from changes in
    /// the underlying data structure.</remarks>
    internal sealed class SubstitutionList {
        private ArrayList s;
        
        public SubstitutionList() {
            this.s = new ArrayList(4);
        }

        /// <summary>
        /// Add a substituion pair
        /// </summary>
        /// <param name="find">a node to be replaced</param>
        /// <param name="replace">its replacement</param>
        public void AddSubstitutionPair(QilNode find, QilNode replace) {
            s.Add(find);
            s.Add(replace);
        }

        /// <summary>
        /// Remove the last a substituion pair
        /// </summary>
        public void RemoveLastSubstitutionPair() {
            s.RemoveRange(s.Count - 2, 2);
        }

        /// <summary>
        /// Remove the last N substitution pairs
        /// </summary>
        public void RemoveLastNSubstitutionPairs(int n) {
            Debug.Assert(n >= 0, "n must be nonnegative");
            if (n > 0) {
                n *= 2;
                s.RemoveRange(s.Count - n, n);
            }
        }

        /// <summary>
        /// Find the replacement for a node
        /// </summary>
        /// <param name="n">the node to replace</param>
        /// <returns>null if no replacement is found</returns>
        public QilNode FindReplacement(QilNode n) {
            Debug.Assert(s.Count % 2 == 0);
            for (int i = s.Count-2; i >= 0; i-=2)
                if (s[i] == n)
                    return (QilNode)s[i+1];
            return null;
        }
    }
}
