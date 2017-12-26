//------------------------------------------------------------------------------
// <copyright file="XmlNavigatorStack.cs" company="Microsoft">
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
using System.Xml;
using System.Xml.XPath;
using System.Diagnostics;

namespace System.Xml.Xsl.Runtime {

    /// <summary>
    /// A dynamic stack of IXmlNavigators.
    /// </summary>
    internal struct XmlNavigatorStack {
        private XPathNavigator[] stkNav;    // Stack of XPathNavigators
        private int sp;                     // Stack pointer (size of stack)

    #if DEBUG
        private const int InitialStackSize = 2;
    #else
        private const int InitialStackSize = 8;
    #endif

        /// <summary>
        /// Push a navigator onto the stack
        /// </summary>
        public void Push(XPathNavigator nav) {
            if (this.stkNav == null)
            {
                this.stkNav = new XPathNavigator[InitialStackSize];
            }
            else
            {
                if (this.sp >= this.stkNav.Length)
                {
                    // Resize the stack
                    XPathNavigator[] stkOld = this.stkNav;
                    this.stkNav = new XPathNavigator[2 * this.sp];
                    Array.Copy(stkOld, this.stkNav, this.sp);
                }
            }

            this.stkNav[this.sp++] = nav;
        }

        /// <summary>
        /// Pop the topmost navigator and return it
        /// </summary>
        public XPathNavigator Pop() {
            Debug.Assert(!IsEmpty);
            return this.stkNav[--this.sp];
        }

        /// <summary>
        /// Returns the navigator at the top of the stack without adjusting the stack pointer
        /// </summary>
        public XPathNavigator Peek() {
            Debug.Assert(!IsEmpty);
            return this.stkNav[this.sp - 1];
        }

        /// <summary>
        /// Remove all navigators from the stack
        /// </summary>
        public void Reset() {
            this.sp = 0;
        }

        /// <summary>
        /// Returns true if there are no navigators in the stack
        /// </summary>
        public bool IsEmpty {
            get { return this.sp == 0; }
        }
    }
}
