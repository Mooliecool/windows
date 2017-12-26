using System;
using System.Collections;

namespace AspNetForums.Components
{
	/// <summary>
	/// Summary description for PostCollection.
	/// </summary>
	public class ModerationAuditCollection : ArrayList
	{
	
		// default constructor
		public ModerationAuditCollection() : base() {}

		public ModerationAuditCollection(ICollection c) : base(c) {}
	}
}
