=============================================================================
  Visual Studio VSPackage : CSVSPackageStatusBar Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The Visual Studio status bar, the horizontal region at the bottom of the 
Visual Studio design surface, provides a convenient way to convey information 
about the current state of the integrated development environment (IDE). 
The status bar comprises four programmable regions, as shown in the following 
table:

Feedback
Displays text. You can set and retrieve text, display static text, and highlight 
the displayed text.

Progress Bar
Displays incremental progress for quick operations, such as saving a single file
 to disk.
 
Animation
Displays a continuously looped animation, which indicates either a lengthy 
operation or an operation of indeterminate length (for example, building multiple 
projects in a solution).

Designer
Displays information pertinent to editing, such as the line number or column 
number of the cursor location.

In this sample, we will demo:
1. Write highlighted text in feedback region
2. Read text from feedback region
3. Show progress bar in status bar
4. Show animation in the status bar
5. Write row, column and char to designer region

/////////////////////////////////////////////////////////////////////////////
Prerequisites:

VS 2008 SDK must be installed on the machine. You can download it from:
http://www.microsoft.com/downloads/details.aspx?FamilyID=30402623-93ca-479a-
867c-04dc45164f5b&displaylang=en

Otherwise the project may not be opened by Visual Studio.

NOTE: The Package Load Failure Dialog occurs because there is no
      PLK(Package Load Key) Specified in this package. To obtain a PLK, please
      to go to WebSite:
      http://msdn.microsoft.com/en-us/vsx/cc655795.aspx
      More info:
      http://msdn.microsoft.com/en-us/library/bb165395.aspx


/////////////////////////////////////////////////////////////////////////////
Creation:

To write the Feedback region of the Visual Studio Status bar
1. Obtain an instance of the IVsStatusbar interface, which is made 
available through the SVsStatusbar service.
2. SetColorText only displays white text on a dark blue background.
3. SetText to set text in the feedback region

To read the Feedback region of the Visual Studio Status bar
1. Obtain an instance of the IVsStatusbar interface, which is made 
available through the SVsStatusbar service.
2. GetText to get the text from the feedback region

To show progress bar in the status bar
1. Obtains an instance of the IVsStatusbar interface from the SVsStatusbar service.
2. Initializes the progress bar to given starting values by calling the Progress 
method.
3. Simulates an operation by iterating through a for loop and updating the progress 
bar values using the Progress method.
4. Clears the progress bar using the Clear method.

To use the animation region of status bar
1. Obtain an instance of the IVsStatusbar interface, which is made available through 
the SVsStatusbar service.
2. Start the animation by calling the Animation method of the status bar. Pass in 1 
as the value of the first parameter, and a reference to an animated icon as the value 
of the second parameter.
3. Stop the animation by calling the Animation method of the status bar. Pass in 0 as 
the value of the first parameter, and a reference to the animated icon as the value of 
the second parameter.

To program the designer region of the status bar
1. Obtain an instance of the IVsStatusbar interface, which is made available through 
the SVsStatusbar service.
2. Update the Designer region of the status bar by calling the SetInsMode and 
SetLineColChar methods of the IVsStatusbar instance.

/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Status Bar
http://msdn.microsoft.com/en-us/library/bb166795.aspx

/////////////////////////////////////////////////////////////////////////////
