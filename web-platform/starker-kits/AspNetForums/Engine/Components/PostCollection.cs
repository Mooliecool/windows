using System;
using System.Collections;

namespace AspNetForums.Components {

    // *********************************************************************
    //
    //  PostCollection
    //
    /// <summary>
    /// Summary description for PostCollection.
    /// </summary>
    //
    // ********************************************************************/    
    public class PostCollection : ArrayList {
        // member variables
        int _totalRecordCount;
		
        // default constructor
        public PostCollection() : base() {}

        public PostCollection(ICollection c) : base(c) {}


        public int TotalRecordCount {
            get { return _totalRecordCount; }
            set {
                if (value < 0)
                    _totalRecordCount = 0;
                else
                    _totalRecordCount = value;
            }
        }
    }
}
