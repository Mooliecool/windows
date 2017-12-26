========================================================================
                  VBASPNETPrintPartOfPage Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The project illustrates how to print a specific part of a page.
A web form page will contain many parts and some of them need not 
print for a page, such as button controls, you can not click them
in print page, So this sample provides a method to avoid print
needless part of page.

/////////////////////////////////////////////////////////////////////////////
Demo the Sample. 

Please follow these demonstration steps below.

Step 1: Open the VBASPNETPrintPartOfPage.sln.

Step 2: Expand the VBASPNETPrintPartOfPage web application and press 
        Ctrl + F5 to show the Default.aspx.

Step 3: You will see many parts of Default.aspx page, There are one "print this
        page" button and four CheckBoxes in the middle of page.

Step 4: Choose the CheckBox to select which part of the page you want to print
        , then click the Button control to print current page. If you do not
	    have an available printer, Choose the MicroSoft XPS Document Writer to 
		test this sample. You can see the part of page print with .xps
	    file, except for the title of web page.

Step 5: Validation finished.

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step 1. Create a VB "ASP.NET Empty Web Application" in Visual Studio 2010 or
        Visual Web Developer 2010. Name it as "VBASPNETPrintPartOfPage".

Step 2. Add a web form in the root direcory, named it as "Default.aspx".

Step 3. Add a "image" folder in the root direcory, add a picture you want 
        display in page.

Step 4. Create some tables in Default.aspx,and you can fill them with html
        elements such as image, text, control , etc. 

Step 5. Define some public strings to store html tag and deposite them in
        Default.aspx page. 
	    [code]
		    'define some stirngs,use to cut of html code
            Public printImageBegin As String
            Public printImageEnd As String

		    'check the status of CheckBox,set div elements.
            If CheckBox2.Checked Then
                printImageBegin = String.Empty
                printImageEnd = String.Empty
            Else
                printImageBegin = enablePirnt
                printImageEnd = endDiv
            End If
		[/code]
         
Step 6. Use JavaScript code to print currently page depend on the status of
        CheckBox, assign JavaScript function to button's onclick event.The
		css and js code:
		[code]
		<style type="text/css" media="print">  
            .nonPrintable
            {
               display: none;
            }
        </style>
        <script type="text/javascript">
            function print_page() {
               window.print();
            }
        </script>
		[/code]

Step 7. Build the application and you can debug it.
/////////////////////////////////////////////////////////////////////////////
References:

MSDN: window.print function
http://msdn.microsoft.com/en-us/library/ms536672(VS.85).aspx

MSDN: CSS Reference
http://msdn.microsoft.com/en-us/library/ms531209(VS.85).aspx
/////////////////////////////////////////////////////////////////////////////