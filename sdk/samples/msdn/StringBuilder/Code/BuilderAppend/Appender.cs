using System;
using System.Text;

namespace BuilderAppend
{
	/// <summary>
	/// Summary description for Appender.
	/// </summary>
	class Appender
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

			Greeting.Append("Good Morning");

			if (Name.Length > 0)
			{
				Greeting.Append(", "+Name+"!");
			}
			else
			{
				Greeting.Append("!");
			}
			
			Console.WriteLine(Greeting);
			Console.Read();

		}
	}
}
