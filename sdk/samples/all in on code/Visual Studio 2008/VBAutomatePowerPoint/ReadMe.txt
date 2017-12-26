========================================================================
    CONSOLE APPLICATION : VBAutomatePowerPoint Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The VBAutomatePowerPoint example demonstrates the use of VB.NET code to 
create a Microsoft PowerPoint instance, add a new presentation, insert a new 
slide, add some texts to the slide, save the presentation, quit PowerPoint 
and then clean up unmanaged COM resources.

Office automation is based on Component Object Model (COM). When you call a 
COM object of Office from managed code, a Runtime Callable Wrapper (RCW) is 
automatically created. The RCW marshals calls between the .NET application 
and the COM object. The RCW keeps a reference count on the COM object. If 
all references have not been released on the RCW, the COM object of Office 
does not quit and may cause the Office application not to quit after your 
automation. In order to make sure that the Office application quits cleanly, 
the sample demonstrates two solutions.

Solution1.AutomatePowerPoint demonstrates automating Microsoft PowerPoint 
application by using Microsoft PowerPoint Primary Interop Assembly (PIA) and 
explicitly assigning each COM accessor object to a new varaible that you 
would explicitly call Marshal.FinalReleaseComObject to release it at the end.

Solution2.AutomatePowerPoint demonstrates automating Microsoft PowerPoint 
application by using Microsoft PowerPoint PIA and forcing a garbage 
collection as soon as the automation function is off the stack (at which 
point the RCW objects are no longer rooted) to clean up RCWs and release COM 
objects.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

You must run this code sample on a computer that has Microsoft PowerPoint 
2007 installed.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the PowerPoint automation 
sample that starts a Microsoft PowerPoint instance, adds a new presentation, 
inserts a slide, adds some texts to the slide, saves the presentation, quits 
PowerPoint and then cleans up unmanaged COM resources.

Step1. After you successfully build the sample project in Visual Studio 2008, 
you will get the application: VBAutomatePowerPoint.exe.

Step2. Open Windows Task Manager (Ctrl+Shift+Esc) to confirm that no 
powerpnt.exe is running. 

Step3. Run the application. It should print the following content in the 
console window if no error is thrown.

  A new presentation is created
  Insert a slide
  Add some texts
  Save and close the presentation
  Quit the PowerPoint application

  A new presentation is created
  Insert a slide
  Add some texts
  Save and close the presentation
  Quit the PowerPoint application

Then, you will see two new presentations in the directory of the application:
Sample1.pptx and Sample2.pptx. Both presentations contain only one slide with 
the following title.

  All-In-One Code Framework

Step4. In Windows Task Manager, confirm that the powerpnt.exe process does 
not exist, i.e. the Microsoft PowerPoint intance was closed and cleaned up 
properly.


/////////////////////////////////////////////////////////////////////////////
Project Relation:

VBAutomatePowerPoint - CSAutomatePowerPoint - CppAutomatePowerPoint

These examples automate Microsoft PowerPoint to do the same thing in  
different programming languages.


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create a Console application and reference the PowerPoint Primary  
Interop Assembly (PIA) and Office 12. To reference the PowerPoint PIA and 
Office 12, right-click the project file and click the "Add Reference..." 
button. In the Add Reference dialog, navigate to the .NET tab, find 
Microsoft.Office.Interop.PowerPoint 12.0.0.0 and Office 12.0.0.0 and click OK.

Step2. Import and rename the Excel interop and the Office namepaces:

	Imports Office = Microsoft.Office.Core
	Imports PowerPoint = Microsoft.Office.Interop.PowerPoint

Step3. Start up a PowerPoint application by creating a PowerPoint.Application 
object.

	oPowerPoint = New PowerPoint.Application()

By default PowerPoint is invisible, till you make it visible.

	' Make the PowerPoint instance invisible
	oPowerPoint.Visible = Office.MsoTriState.msoFalse
	' Or make the PowerPoint instance visible
	oPowerPoint.Visible = Office.MsoTriState.msoTrue

Step4. Get the Presentations collection from Application.Presentations and 
call its Add function to add a new presentation. The Add function returns a 
Presentation object.

	oPres = oPowerPoint.Presentations
	oPre = oPres.Add()

Step5. Insert a slide by calling the Add method on the Presentation.Slides 
collection, and add some texts to the slide.

	oSlides = oPre.Slides
	oSlide = oSlides.Add(1, PowerPoint.PpSlideLayout.ppLayoutText)

	oShapes = oSlide.Shapes
	oShape = oShapes(1)
	oTxtFrame = oShape.TextFrame
	oTxtRange = oTxtFrame.TextRange
	oTxtRange.Text = "All-In-One Code Framework"

Step6. Save the presentation as a pptx file and close it.

	Dim fileName As String = Path.GetDirectoryName( _
	Assembly.GetExecutingAssembly().Location) + "\\Sample1.pptx"
	oPre.SaveAs(fileName, PowerPoint.PpSaveAsFileType.ppSaveAsOpenXMLPresentation, _
			Office.MsoTriState.msoTriStateMixed)
	oPre.Close()

Step7. Quit the PowerPoint application.

	oPowerPoint.Quit()

Step8. Clean up the unmanaged COM resource. To get PowerPoint terminated 
rightly, we need to call Marshal.FinalReleaseComObject() on each COM object 
we used. We can either explicitly call Marshal.FinalReleaseComObject on all 
accessor objects:

	' See Solution1.AutomatePowerPoint
	If Not oTxtRange Is Nothing Then
		Marshal.FinalReleaseComObject(oTxtRange)
		oTxtRange = Nothing
	End If
	If Not oTxtFrame Is Nothing Then
		Marshal.FinalReleaseComObject(oTxtFrame)
		oTxtFrame = Nothing
	End If
	If Not oShape Is Nothing Then
		Marshal.FinalReleaseComObject(oShape)
		oShape = Nothing
	End If
	If Not oShapes Is Nothing Then
		Marshal.FinalReleaseComObject(oShapes)
		oShapes = Nothing
	End If
	If Not oSlide Is Nothing Then
		Marshal.FinalReleaseComObject(oSlide)
		oSlide = Nothing
	End If
	If Not oSlides Is Nothing Then
		Marshal.FinalReleaseComObject(oSlides)
		oSlides = Nothing
	End If
	If Not oPre Is Nothing Then
		Marshal.FinalReleaseComObject(oPre)
		oPre = Nothing
	End If
	If Not oPres Is Nothing Then
		Marshal.FinalReleaseComObject(oPres)
		oPres = Nothing
	End If
	If Not oPowerPoint Is Nothing Then
		Marshal.FinalReleaseComObject(oPowerPoint)
		oPowerPoint = Nothing
	End If

and/or force a garbage collection as soon as the calling function is off the 
stack (at which point these objects are no longer rooted) and then call 
GC.WaitForPendingFinalizers.

	' See Solution2.AutomatePowerPoint
	GC.Collect();
	GC.WaitForPendingFinalizers();
	' GC needs to be called twice in order to get the Finalizers called 
	' - the first time in, it simply makes a list of what is to be 
	' finalized, the second time in, it actually is finalizing. Only 
	' then will the object do its automatic ReleaseComObject.
	GC.Collect();
	GC.WaitForPendingFinalizers();


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: PowerPoint 2007 Developer Reference
http://msdn.microsoft.com/en-us/library/bb265982.aspx


/////////////////////////////////////////////////////////////////////////////
