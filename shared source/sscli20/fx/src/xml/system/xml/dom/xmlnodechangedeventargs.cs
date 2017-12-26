//------------------------------------------------------------------------------
// <copyright file="XmlNodeChangedEventArgs.cs" company="Microsoft">
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

namespace System.Xml {
    public class XmlNodeChangedEventArgs : EventArgs {
        private XmlNodeChangedAction    action;
        private XmlNode                 node;
        private XmlNode                 oldParent;
        private XmlNode                 newParent;
        private string                  oldValue;
        private string                  newValue;

        public XmlNodeChangedEventArgs( XmlNode node, XmlNode oldParent, XmlNode newParent, string oldValue, string newValue, XmlNodeChangedAction action ) {
            this.node = node;
            this.oldParent = oldParent;
            this.newParent = newParent;
            this.action = action;
            this.oldValue = oldValue;
            this.newValue = newValue;
        }

        public XmlNodeChangedAction Action { get { return action; } }

        public XmlNode Node { get { return node; } }

        public XmlNode OldParent { get { return oldParent; } }

        public XmlNode NewParent { get { return newParent; } }

        public string OldValue { get { return oldValue; } }

        public string NewValue { get { return newValue; } }
    }
}
