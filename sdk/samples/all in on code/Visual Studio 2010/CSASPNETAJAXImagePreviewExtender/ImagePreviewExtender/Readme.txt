========================================================================
              CSASPNETAJAXImagePreviewExtender Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The project illustrates how to design an AJAX Control Extender. 
In this sample, it is one extender for images.
The images which use the extender control will be shown in a thumbnail mode at
first, if user click the image, a big picture will be popped up and show the
true size of the image.

/////////////////////////////////////////////////////////////////////////////
Demo the Sample. 

Open the CSASPNETAJAXImagePreviewExtender.sln directly and open the 
ImagePreviewExtenderTest website to test the page extender directly.

If you want to have a further test, please follow the demonstration step below.

Step 1: View the Default.aspx in your browser. You will find some pictures.

Step 2: When the page is loaded, the pictures are all in thumbnail mode, it 
looks small. Click on the first image.

Step 3: When click on the image which using the extender, you will see a big
popup image with the whole size and the position of the picture will be auto-
fitted.

Step 4: Click the CLOSE button at the left-top of the big image.

Step 5: If we put the image into a Panel and the Panel has been extended by 
ImagePreviewExtender, you will find all the images in this panel will get the
same feature.

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step 1.  Create a C# "ASP.NET AJAX Server Control Extender" Project 
	in Visual Studio 2010 or Visual Web Developer 2010.
	Name it as ImagePreviewExtender.

Step 2. We can see three files created. one .JS file, one .CS and 
	one .RESX file. Delete the .RESX file, we don't need that here.
	Rename the js file to ImagePreviewBehavior.js.
	Rename the cs file to ImagePreviewControl.cs.

Step 3.  Open the ImagePreviewControl.cs.
	Add a property called "ThumbnailCssClass".
	In the overrides function GetScriptDescriptors, use code like below to
	add the property to the descriptor.

	[Code]
	ScriptBehaviorDescriptor descriptor = new ScriptBehaviorDescriptor(
		"ImagePreviewExtender.ImagePreviewBehavior", 
		targetControl.ClientID);
        descriptor.AddProperty("ThumbnailCssClass", ThumbnailCssClass);
	[/Code]

	Modify the name of the ScriptReference in the function of 
	GetScriptReferences.

Step 4.  Open the ImagePreviewBehavior.js. Write the javascript according to 
	the description in the sample file with the same file name.

Step 5.  Open the AssemblyInfo.cs in the folder of "Properties". At the bottom 
	of the file, create two web resource defination. We can download a close
	icon and copy the file to the root directory.

	[Code]
	[Assembly: WebResource("ImagePreviewExtender.ImagePreviewBehavior.js",
			 "text/javascript")] 
	[Assembly: WebResource("ImagePreviewExtender.Close.png", 
			"image/png")]
	[/Code]

Step 6.  The extender is ready. Create a ASP.NET WebSite or ASP.NET Web Application
	to test the extender. Reference the project at first and then use the control
	just like we use one in the AJAXControlToolKit. 



/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Microsoft Ajax Extender Controls Overview
http://msdn.microsoft.com/en-us/library/bb470384.aspx

MSDN: Walkthrough: Microsoft Ajax Extender Controls
http://msdn.microsoft.com/en-us/library/bb470455.aspx

MSDN: Creating an Extender Control to Associate a Client Behavior with a Web Server Control
http://msdn.microsoft.com/en-us/library/bb386403.aspx

MSDN: ExtenderControl Members
http://msdn.microsoft.com/en-us/library/system.web.ui.extendercontrol_members.aspx

/////////////////////////////////////////////////////////////////////////////