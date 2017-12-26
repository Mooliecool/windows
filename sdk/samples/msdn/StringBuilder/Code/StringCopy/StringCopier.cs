using System;

namespace StringCopy
{
	/// <summary>
	/// Summary description for StringCopier.
	/// </summary>
	class StringCopier
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
			Console.WriteLine("An Example of String Copy");
			Console.WriteLine("Type Some Text");
			string inputText = Console.ReadLine();
			string copiedText = string.Copy(inputText);

			copiedText+=" more text";
			Console.WriteLine("Your text is here: "+inputText);
			Console.WriteLine("Copy of your text with some additions: "+copiedText);
			Console.Read();
		}
	}
}
