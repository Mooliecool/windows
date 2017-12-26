using System;
using System.Collections;

namespace AspNetForums.Components
{
	/// <summary>
	/// Summary description for ModeratedForumCollection.
	/// </summary>
	public class ModeratedForumCollection : ArrayList
	{
		public ModeratedForumCollection() : base() {}
		public ModeratedForumCollection(ICollection c) : base(c) {}
	}
}
