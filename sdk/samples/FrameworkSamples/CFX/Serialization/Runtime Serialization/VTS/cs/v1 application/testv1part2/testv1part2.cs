using System;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;
using Microsoft.Samples.Test;

namespace Microsoft.Samples
{
	/// <summary>
	/// Summary description for Class1.
	/// </summary>
	class TestV1
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			//
			// TODO: Add code to start application here
			//
			BinaryFormatter formatter = new BinaryFormatter();
			FileStream v2File = null;
			try
			{

				v2File = new FileStream(@"..\..\..\..\Output\v2Output.bin", FileMode.Open);
				Person aV2Person = (Person)formatter.Deserialize(v2File);

				Console.WriteLine("\nPerson values after deserialize from v2:");
				Console.WriteLine("\tName: " + aV2Person.Name);
				Console.WriteLine("\tAddress: " + aV2Person.Address);
				Console.WriteLine("\tBirth Date: " + aV2Person.BirthDate.ToShortDateString());
			}
			catch (FileNotFoundException e)
			{
				Console.WriteLine("An error occurred:\n" + e.ToString());
			}
			finally
			{
				if (v2File != null)
				{
					v2File.Close();
				}
			}
		}
	}
}
