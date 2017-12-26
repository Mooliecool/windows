//------------------------------------------------------------------------------
// <copyright file="XmlNodeList.cs" company="Microsoft">
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
    using System.Collections;

    // Represents an ordered collection of nodes.
    public abstract class XmlNodeList: IEnumerable {

        // Retrieves a node at the given index.
        public abstract XmlNode Item(int index);

        // Gets the number of nodes in this XmlNodeList.
        public abstract int Count { get;}

        // Provides a simple ForEach-style iteration over the collection of nodes in
        // this XmlNodeList.
        public abstract IEnumerator GetEnumerator();

        // Retrieves a node at the given index.
        [System.Runtime.CompilerServices.IndexerName ("ItemOf")]
        public virtual XmlNode this[int i] { get { return Item(i);}}
    }
}

