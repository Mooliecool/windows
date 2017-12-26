using System;
using System.Collections;

namespace AspNetForums.Components {
    /// <summary>
    /// Summary description for PostCollection.
    /// </summary>
    public class UserCollection : ArrayList {
	
        // default constructor
        public UserCollection() : base() {}
        public UserCollection(ICollection c) : base(c) {}
    }
}
