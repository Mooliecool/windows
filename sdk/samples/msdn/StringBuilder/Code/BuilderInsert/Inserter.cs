using System;
using System.Text;

namespace BuilderInsert
{
	/// <summary>
	/// Summary description for Inserter.
	/// </summary>
	class Inserter
	{
		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main(string[] args)
		{
			Console.WriteLine("Please Enter Your Name:");
			string Name = Console.ReadLine();
			StringBuilder Greeting = new StringBuilder();

			Greeting.Append("Good Morning!");

			if (Name.Length > 0)
			{
				Greeting.Insert(12,", "+ Name);
			}

			Console.WriteLine(Greeting);
			Console.Read();
		}
	}
}
