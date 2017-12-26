using System;
using System.Collections;

namespace AspNetForums.Components
{
	/// <summary>
	/// Summary description for ForumCollection.
	/// </summary>
	public class ForumCollection : ArrayList
	{
		public ForumCollection() : base() {}
		public ForumCollection(ICollection c) : base(c) {}
	}
}
