//-----------------------------------------------------------------------
//  This file is part of the Microsoft .NET Framework SDK Code Samples.
// 
//  Copyright (C) Microsoft Corporation.  All rights reserved.
// 
//This source code is intended only as a supplement to Microsoft
//Development Tools and/or on-line documentation.  See these other
//materials for detailed information regarding Microsoft code samples.
// 
//THIS CODE AND INFORMATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY
//KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
//PARTICULAR PURPOSE.
//-----------------------------------------------------------------------
// Filename: wordapp.cs
//
// C# application demonstrating the use of the Microsoft Word object model

using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Reflection;
using System.Collections;
using System.Threading;
using Microsoft.Office.Interop.Word;

namespace Microsoft.Samples.Interop.WordApp
{
	class WordAppMain
    {
		static object missing = Missing.Value;
		static object missing2 = Missing.Value;
		static object missing3 = Missing.Value;
		static object missing4 = Missing.Value;
		static object missing5 = Missing.Value;
		static object missing6 = Missing.Value;
		static object missing7 = Missing.Value;
		static object missing8 = Missing.Value;
		static object missing9 = Missing.Value;
		static object missing10 = Missing.Value;
		static object missing11 = Missing.Value;
		static object missing12 = Missing.Value;
		static object missing13 = Missing.Value;


		static int Main()
    	{
			
			WordAppMain myWord = new WordAppMain();
			int return_Result = 0;
			
			// Create a word object that we can manipulate
			Application Word_App = null;
			Document Word_doc=null;
			try
			{
				Word_App = new Application();
				Word_doc=new Document();
			}
			catch(Exception e)
			{
				Console.WriteLine("Can't create a word document " + e.ToString());
				return_Result = 1;
				goto Exit;
			}

			AutoCorrect autocorrect = Word_App.AutoCorrect;
			AutoCorrectEntries autoEntries = autocorrect.Entries; 

			string theEnd= "\nThe End";
			autoEntries.Add("Inntroduction", "Introduction");

			Documents Docs = Word_App.Documents;
			if (Docs == null)
			{
				Console.WriteLine("Docs is null");
			}
			else
			{	
				Console.WriteLine("Docs exists:" + Docs.Count);
			}

			Word_App.Visible=true;
			_Document my_Doc= (_Document) Word_doc;
			Word_doc=Docs.Add(ref missing, ref missing, ref missing, ref missing);

			object start = 0;
			object end = 0;
			Range range = Word_doc.Range(ref missing,ref missing);	
			
			// add text to the doc -- this contains some deliberate misspellings so that we can correct them in a short while
			range.Text="Microsoft Word Interoperability Sample\n\nInntroduction:\n\nMicrosoft .NET will allow the creation of truly distributed XML Web services. These services will integrate and collaborate with a range of complementary services to work for customers in ways that today's internet companies can only dream of. Microsoft .NET will drive the Next Generation Internet and will shift the focus from individual Web sites or devices connected to the Internet, to constellations of computers, devices, and services that work together to deliver broader, richer solutions.\nFor more info go to:\n   ";

			// Wait so the starting state can be admired
			Thread.Sleep(2000);

			// Format the title
			Font fc= new Font();
			try
			{
				Console.WriteLine("Formatting the title");
				start = 0; end = 40;
				range=Word_doc.Range(ref start, ref end);
				range.Font.Size=24;
				range.Font.Bold=1; 
				range.Font.Color=WdColor.wdColorGray30;
				start = 40; end = 54;
				range=Word_doc.Range(ref start, ref end);
				range.Font.Size=14;
				

			}
			catch(Exception e)
			{
				Console.WriteLine(" Font exception:{0}", e.ToString());
			}


			// Wait so the new formatting can be appreciated
			Thread.Sleep(3000);

			autocorrect.ReplaceTextFromSpellingChecker=true;
			// Fix inntroduction
			string obj = "Introduction";
			AutoCorrectEntry errEntry= autoEntries.Add("Inntroduction", obj);

			Words myWords=Word_doc.Words;
			Range errRange= myWords[7];
			errEntry.Apply(errRange);

			// Add a caption to the window and get it back 			
			Window myWindow = Word_App.ActiveWindow;
			myWindow.Caption = "Managed Word execution from C# ";
			string gotCaption = myWindow.Caption;
			if (gotCaption.Equals("Managed Word execution from C# "))
			{
				Console.WriteLine("Caption assigned and got back");
				return_Result = 1;
			}
			Thread.Sleep(2000);

			// define the selection object, find and  replace text
			Selection mySelection = myWindow.Selection;

			try
			{
				start = 65; end = 69;
				range=Word_doc.Range(ref start, ref end);
				Console.WriteLine("The color of .NET is being changed");
				
				range.Font.Bold=16;
				range.Font.Color=WdColor.wdColorLavender;

			}
			catch(Exception e)
			{
				Console.WriteLine(" Font exception:{0}", e.ToString());
			}
			Thread.Sleep(2000);
			
			// underline the selected text
 			range=Word_doc.Range(ref start,ref end);
			range.Underline=(WdUnderline.wdUnderlineDouble);

			// add hyperlink and follow the hyperlink
			Hyperlinks my_Hyperlinks = Word_doc.Hyperlinks;

			// Make the range past the end of all document text
			mySelection.Start = 9999;
			mySelection.End   = 9999;
			range = mySelection.Range;

			// Add a hyperlink
			string myAddress = "http://go.microsoft.com/fwlink/?linkid=3269&clcid=0x409";
			object obj_Address = myAddress;
			Console.WriteLine("Adding hyperlink to the document");
			Hyperlink my_Hyperlink1= my_Hyperlinks._Add(range, ref obj_Address, ref missing);  			
			Word_App.ActiveWindow.Selection.InsertAfter("\n");

			Thread.Sleep(5000);

			// Open a window to Hyperlink
			Process ie = Process.Start("iexplore.exe", my_Hyperlink1.Address);			

			// Wait for a short spell to allow the page to be examined
			Thread.Sleep(10000);
			
			// close the browser first
			Console.WriteLine("Removing browser window");
			ie.Kill();

			// Display "The End"
			Word_App.ActiveWindow.Selection.InsertAfter(theEnd);
			Word_App.ActiveWindow.Selection.Start = 0;
			Word_App.ActiveWindow.Selection.End = 0;
			Word_App.Activate();
			Thread.Sleep(5000);

			// Close Microsoft Word
			object myBool = WdSaveOptions.wdDoNotSaveChanges;
			Word_App.ActiveWindow.Close(ref myBool,ref missing);
		Exit:
			return return_Result;
	    }
	}
}
