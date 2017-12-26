using System;

namespace StringSubstring
{
	/// <summary>
	/// Summary description for Class1.
	/// </summary>
	class Substring
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
			
			string originalString = "abcdefghijklmnop";
			
			Console.WriteLine("A Substring example:");
			Console.WriteLine("String to Be Searched: " + originalString);
            string returnedString = originalString.Substring(4, 3).ToString();
			Console.WriteLine("Start Index: " + "4");
			Console.WriteLine("Length: " + "3");
			Console.WriteLine("Substring: " + returnedString);
			Console.Read();
		}
	}
}
