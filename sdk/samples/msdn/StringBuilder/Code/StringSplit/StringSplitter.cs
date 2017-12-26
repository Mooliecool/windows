using System;

namespace StringSplit
{
	/// <summary>
	/// Summary description for StringSplitter.
	/// </summary>
	class StringSplitter
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
			string textOriginal = "Value 1, Value 2, Value 3";

			string[] textArray = textOriginal.Split(',');
			
			Console.WriteLine("An example of splitting a String:\r\n\r\n");
			Console.WriteLine("The string to split: "+textOriginal);
			Console.WriteLine("The character to split from: ','");
			Console.WriteLine("\r\nResults:");
			foreach(string newText in textArray)
			{
				Console.WriteLine(newText.Trim().ToString());
			}

			Console.WriteLine("");
			Console.WriteLine("\r\n\r\nNow rejoin the textarray with a different delimiter:");			
			//now rejoin the array of strings
			string newJoin = string.Join(":", textArray);
			Console.WriteLine(newJoin);
			Console.Read();
			
		}
	}
}
