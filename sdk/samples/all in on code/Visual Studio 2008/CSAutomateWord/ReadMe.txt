========================================================================
    CONSOLE APPLICATION : CSAutomateWord Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The CSAutomateWord example demonstrates the use of Visual C# code to create a 
Microsoft Word instance, create a new document, insert a paragraph and a 
table, save the document, close the Microsoft Word application and then 
clean up unmanaged COM resources.

Office automation is based on Component Object Model (COM). When you call a 
COM object of Office from managed code, a Runtime Callable Wrapper (RCW) is 
automatically created. The RCW marshals calls between the .NET application 
and the COM object. The RCW keeps a reference count on the COM object. If 
all references have not been released on the RCW, the COM object of Office 
does not quit and may cause the Office application not to quit after your 
automation. In order to make sure that the Office application quits cleanly, 
the sample demonstrates two solutions.

Solution1.AutomateWord demonstrates automating Microsoft Word application by 
using Microsoft Word Primary Interop Assembly (PIA) and explicitly assigning 
each COM accessor object to a new varaible that you would explicitly call 
Marshal.FinalReleaseComObject to release it at the end.

Solution2.AutomateWord demonstrates automating Microsoft Word application by 
using Microsoft Word PIA and forcing a garbage collection as soon as the 
automation function is off the stack (at which point the RCW objects are no 
longer rooted) to clean up RCWs and release COM objects.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

You must run this code sample on a computer that has Microsoft Word 2007 
installed.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the Word automation 
sample that starts a Microsoft Word instance, creates a new document, 
inserts a paragraph and a table, saves the document, and quits the Microsoft 
Word application cleanly.

Step1. After you successfully build the sample project in Visual Studio 2008, 
you will get the application: CSAutomateWord.exe.

Step2. Open Windows Task Manager (Ctrl+Shift+Esc) to confirm that no 
winword.exe is running. 

Step3. Run the application. It should print the following content in the 
console window if no error is thrown.

  Word.Application is started
  A new document is created
  Insert a paragraph
  Save and close the document
  Quit the Word application

  Word.Application is started
  Insert a paragraph
  Insert a table
  Save and close the document
  Quit the Word application

Then, you will see two new documents in the directory of the application: 
Sample1.docx and Sample2.docx. Both documents have the following content.

  Heading 1

Sample2.docx additionally has this table in the document.

  r1c1	r1c2
  r2c1	r2c2
  r3c1	r3c2
  r4c1	r4c2
  r5c1	r5c2

Step4. In Windows Task Manager, confirm that the winword.exe process does not 
exist, i.e. the Microsoft Word intance was closed and cleaned up properly.


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CSAutomateWord - VBAutomateWord - CppAutomateWord

These examples automate Microsoft Word to do the same thing in different 
programming languages.


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create a Console application and reference the Word Primary Interop 
Assembly (PIA). To reference the Word PIA, right-click the project file
and click the "Add Reference..." button. In the Add Reference dialog, 
navigate to the .NET tab, find Microsoft.Office.Interop.Word 12.0.0.0 and 
click OK.

Step2. Import and rename the Excel interop namepace:

	using Word = Microsoft.Office.Interop.Word;

Step3. Start up a Word application by creating a Word.Application object.

	oWord = new Word.Application();

Step4. Get the Documents collection from Application.Documents and call its 
Add function to create a new document. The Add function returns a Document 
object.

Step5. Insert a paragraph.

	oParas = oDoc.Paragraphs;
	oPara = oParas.Add(ref missing);
	oParaRng = oPara.Range;
	oParaRng.Text = "Heading 1";
	oFont = oParaRng.Font;
	oFont.Bold = 1;
	oParaRng.InsertParagraphAfter();

Step6. Insert a table.

The following code has the problem that it invokes accessors which will also 
create RCWs and reference them. For example, calling Document.Bookmarks.Item 
creates an RCW for the Bookmarks object. If you invoke these accessors via 
tunneling as this code does, the RCWs are created on the GC heap, but the 
references are created under the hood on the stack and are then discarded. As 
such, there is no way to call MarshalFinalReleaseComObject on those RCWs. To 
get them to release, you would either need to force a garbage collection as 
soon as the calling function is off the stack (at which point these objects 
are no longer rooted) and then call GC.WaitForPendingFinalizers, or you would 
need to change the syntax to explicitly assign these accessor objects to a 
variable that you would then explicitly call Marshal.FinalReleaseComObject on. 

	oBookmarkRng = oDoc.Bookmarks.get_Item(ref oEndOfDoc).Range;
	oTable = oDoc.Tables.Add(oBookmarkRng, 5, 2, ref missing, ref missing);
	oTable.Range.ParagraphFormat.SpaceAfter = 6;
	for (int r = 1; r <= 5; r++)
	{
		for (int c = 1; c <= 2; c++)
		{
			oTable.Cell(r, c).Range.Text = "r" + r + "c" + c;
		}
	}

	// Change width of columns 1 & 2
	oTable.Columns[1].Width = oWord.InchesToPoints(2);
	oTable.Columns[2].Width = oWord.InchesToPoints(3);

Step7. Save the document as a docx file and close it.

	object fileName = Path.GetDirectoryName(
		Assembly.GetExecutingAssembly().Location) + "\\Sample1.docx";
	object fileFormat = Word.WdSaveFormat.wdFormatXMLDocument;
	oDoc.SaveAs(ref fileName, ref fileFormat, ref missing,
		ref missing, ref missing, ref missing, ref missing,
		ref missing, ref missing, ref missing, ref missing,
		ref missing, ref missing, ref missing, ref missing,
		ref missing);
	((Word._Document)oDoc).Close(ref missing, ref missing, ref missing);
                    
Step8. Quit the Word application.

	((Word._Application)oWord).Quit(ref notTrue, ref missing, ref missing);

Step9. Clean up the unmanaged COM resource. To get Word terminated rightly, 
we need to call Marshal.FinalReleaseComObject() on each COM object we used.
We can either explicitly call Marshal.FinalReleaseComObject on all accessor 
objects:

	// See Solution1.AutomateWord
	if (oFont != null)
	{
		Marshal.FinalReleaseComObject(oFont);
		oFont = null;
	}
	if (oParaRng != null)
	{
		Marshal.FinalReleaseComObject(oParaRng);
		oParaRng = null;
	}
	if (oPara != null)
	{
		Marshal.FinalReleaseComObject(oPara);
		oPara = null;
	}
	if (oParas != null)
	{
		Marshal.FinalReleaseComObject(oParas);
		oParas = null;
	}
	if (oDoc != null)
	{
		Marshal.FinalReleaseComObject(oDoc);
		oDoc = null;
	}
	if (oDocs != null)
	{
		Marshal.FinalReleaseComObject(oDocs);
		oDocs = null;
	}
	if (oWord != null)
	{
		Marshal.FinalReleaseComObject(oWord);
		oWord = null;
	}

and/or force a garbage collection as soon as the calling function is off the 
stack (at which point these objects are no longer rooted) and then call 
GC.WaitForPendingFinalizers.

	// See Solution2.AutomateWord
	GC.Collect();
	GC.WaitForPendingFinalizers();
	// GC needs to be called twice in order to get the Finalizers called 
	// - the first time in, it simply makes a list of what is to be 
	// finalized, the second time in, it actually is finalizing. Only 
	// then will the object do its automatic ReleaseComObject.
	GC.Collect();
	GC.WaitForPendingFinalizers();


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Word 2007 Developer Reference
http://msdn.microsoft.com/en-us/library/bb244391.aspx

How to automate Microsoft Word to create a new document by using Visual C#
http://support.microsoft.com/kb/316384

The Designer Process That Would Not Terminate (Part 2) 
http://blogs.msdn.com/geoffda/archive/2007/09/07/the-designer-process-that-would-not-terminate-part-2.aspx


/////////////////////////////////////////////////////////////////////////////
