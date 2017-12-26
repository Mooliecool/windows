// ==++==
// 
//   
//    Copyright (c) 2006 Microsoft Corporation.  All rights reserved.
//   
//    The use and distribution terms for this software are contained in the file
//    named license.txt, which can be found in the root of this distribution.
//    By using this software in any fashion, you are agreeing to be bound by the
//    terms of this license.
//   
//    You must not remove this notice, or any other, from this software.
//   
// 
// ==--==
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

namespace System.Collections {

    using System;

    // Useful base class for typed readonly collections where items derive from object
	[Serializable]
[System.Runtime.InteropServices.ComVisible(true)]
    public abstract class ReadOnlyCollectionBase : ICollection {
        ArrayList list;

        protected ArrayList InnerList {
            get { 
                if (list == null)
                    list = new ArrayList();
                 return list; 
            }
        }

        public virtual int Count {
            get { return InnerList.Count; }
        }

        bool ICollection.IsSynchronized {
            get { return InnerList.IsSynchronized; }
        }

        object ICollection.SyncRoot {
            get { return InnerList.SyncRoot; }
        }

        void ICollection.CopyTo(Array array, int index) {
            InnerList.CopyTo(array, index);
        }

        public virtual IEnumerator GetEnumerator() {
            return InnerList.GetEnumerator();
        }
    }

}
