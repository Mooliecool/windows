using System;
using System.Text;

namespace BuilderReplace
{
	/// <summary>
	/// Summary description for Replacer.
	/// </summary>
	class Replacer
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

			Console.WriteLine("Now Enter a Nickname:");
			string NickName = Console.ReadLine();
			
			//for this example the name and nickName lengths must be supplied.
			if(Name.Length > 0 && NickName.Length > 0)
			{
				Greeting.Replace(Name, NickName);
			}
			
			Console.WriteLine(Greeting);
			Console.Read();

		}
	}
}
