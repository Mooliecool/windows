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
	class TestV2
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
			Person aPerson = null;
			FileStream v1File = null;
			try
			{
				v1File = new FileStream(@"..\..\..\..\Output\v1Output.bin", FileMode.Open);
				aPerson = (Person)formatter.Deserialize(v1File);
				aPerson.Age = 98;
			}
			catch (FileNotFoundException e)
			{
				Console.WriteLine("An error occured:\n" + e.ToString());
			}
			finally
			{
				if (v1File != null)
				{
					v1File.Close();
				}
			}


			FileStream v2File = null;
			
			try {

				v2File = new FileStream(@"..\..\..\..\Output\v2Output.bin", FileMode.Create);
				if (aPerson != null)
				{
					formatter.Serialize(v2File, aPerson);

					Console.WriteLine();
					Console.WriteLine(@"Person v2 written out to ..\..\..\..\Output\v2Output.bin");
				}
			}
			catch (FileNotFoundException e)
			{
				Console.WriteLine("An error occured:\n" + e.ToString());
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
