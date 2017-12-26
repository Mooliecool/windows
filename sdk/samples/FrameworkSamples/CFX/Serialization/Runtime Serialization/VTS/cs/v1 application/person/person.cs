using System;

namespace Microsoft.Samples.Test
{
	/// <summary>
	/// Version 1 of the Person class
	/// </summary>
	/// 
	[Serializable]
	public class Person
	{

		#region Constructors
		public Person()
		{
		}
		#endregion

		#region Fields
		private string _name;
		private string _address;

		private DateTime _birthDate;
		#endregion

		#region Properties
		public string Name
		{
			get { return _name; }
			set { _name = value; }
		}

		public string Address
		{
			get { return _address; }
			set { _address = value; }
		}

		public DateTime BirthDate
		{
			get { return _birthDate; }
			set { _birthDate = value; }
		}
		#endregion

	}
}
