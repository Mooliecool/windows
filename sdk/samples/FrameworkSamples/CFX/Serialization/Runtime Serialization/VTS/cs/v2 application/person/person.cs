using System;
using System.Runtime.Serialization;

namespace Microsoft.Samples.Test
{
	/// <summary>
	/// Version 2 of the Person class.
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

		[OptionalField(VersionAdded = 2)]
		private int _age;
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

		public int Age
		{
			get { return _age; }
			set { _age = value; }
		}

		#endregion

		#region Serialization Processing

		[OnDeserializing]
		private void OnDeserializing(StreamingContext context)
		{
			Console.WriteLine("\nDeserializing into a v2 type of Person.");

			_age = 0;

			Console.WriteLine("\n\tDefaults set:");
			Console.WriteLine("\t\tAge: " + _age);
		}

		[OnDeserialized]
		private void OnDeserialized(StreamingContext context)
		{
			Console.WriteLine("\nDeserialization completed.");
			if (_age == 0)
			{
				_age = DateTime.Now.Year - _birthDate.Year;
				if (DateTime.Now.DayOfYear < _birthDate.DayOfYear) _age--;
			}

			Console.WriteLine("\n\tFixups completed.");
			Console.WriteLine("\tMy Person v2 values are: ");
			Console.WriteLine("\t\tName: " + _name);
			Console.WriteLine("\t\tAddress: " + _address);
			Console.WriteLine("\t\tBirthDate: " + _birthDate.ToShortDateString());
			Console.WriteLine("\t\tAge: " + _age);
		}

		[OnSerializing]
		private void OnSerializing(StreamingContext context)
		{
			Console.WriteLine("\nOnSerializing called.");
		}

		[OnSerialized]
		private void OnSerialized(StreamingContext context)
		{
			Console.WriteLine("\nOnSerialized called.");
		}

		#endregion



	}
}
