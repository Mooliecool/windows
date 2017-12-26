using System;
using System.Collections;

namespace AspNetForums.Components {

    // *********************************************************************
    //
    //  ThreadCollection
    //
    /// <summary>
    /// A collection of threads, i.e. top level posts
    /// </summary>
    //
    // ********************************************************************/    
    public class ThreadCollection : ArrayList {
        // member variables
        int _totalRecordCount;
		
        // default constructor
        public ThreadCollection() : base() {}

        public ThreadCollection(ICollection c) : base(c) {}


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
