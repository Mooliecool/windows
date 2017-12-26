using System;
using System.Collections;

namespace AspNetForums.Components {
    /// <summary>
    /// Summary description for PostCollection.
    /// </summary>
    public class ForumMessageTemplateCollection : ArrayList {
	
        // default constructor
        public ForumMessageTemplateCollection() : base() {}

        public ForumMessageTemplateCollection(ICollection c) : base(c) {}
    }
}
