using System;
using System.Collections;

namespace AspNetForums.Components
{
	/// <summary>
	/// Summary description for PostCollection.
	/// </summary>
	public class ModeratorCollection : ArrayList
	{
	
		// default constructor
		public ModeratorCollection() : base() {}

		public ModeratorCollection(ICollection c) : base(c) {}
	}
}
