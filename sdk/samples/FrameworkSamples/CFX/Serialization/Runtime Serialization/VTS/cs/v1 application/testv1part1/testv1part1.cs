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
			FileStream v1File = null;
			try
			{

				v1File = new FileStream(@"..\..\..\..\Output\v1Output.bin", FileMode.Create);
				Person aPerson = new Person();
				aPerson.Name = "Johnathan";
				aPerson.Address = "102 Main Street";
				aPerson.BirthDate = new DateTime(1980, 1, 31, 1, 0, 0, DateTimeKind.Local);

				formatter.Serialize(v1File, aPerson);
			}
			catch (FileNotFoundException e)
			{
				Console.WriteLine("An error occurred:\n" + e.ToString());
			}
			finally
			{
				if (v1File != null)
				{
					v1File.Close();
				}
			}

			Console.WriteLine();
			Console.WriteLine(@"Person v1 written out to ..\..\..\..\Output\v1Output.bin");

		}
	}
}
