using System;
using System.Collections;

namespace AspNetForums.Components {

    // *********************************************************************
    //
    //  VoteResultCollection
    //
    /// <summary>
    /// Summary description for VoteResultCollection.
    /// </summary>
    //
    // ********************************************************************/    
    public class VoteResultCollection : Hashtable {
        // default constructor
        public VoteResultCollection() : base() {}
        public VoteResultCollection(IDictionary d) : base(d) {}

        public VoteResult this[string key] {
            get {
                return (VoteResult) base[key];
            }
            set {
                base[key] = value;
            }
        }

        public VoteResult this[int index] {
            get {
                return (VoteResult) base[index];
            }
            set {
                base[index] = value;
            }
        }
    }
}
