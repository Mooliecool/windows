using System;
using System.Collections;

namespace AspNetForums.Components {

    /// <summary>
	/// Summary description for ForumCollection.
	/// </summary>
	public class ForumGroupCollection : ArrayList {
		public ForumGroupCollection() : base() {}
		public ForumGroupCollection(ICollection c) : base(c) {}
	}
}
