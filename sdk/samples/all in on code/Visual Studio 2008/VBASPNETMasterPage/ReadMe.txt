========================================================================
      ASP.NET APPLICATION : VBASPNETMasterPage Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

 The project illustrates how to use Master Page. ASP.NET defines two new 
 specialized types of pages: Master Page and Content Page. A Master page 
 is a page template. Like an ordinary ASP.NET web page, it can contain any
 combination of HTML. In addition, Master Page includes a special control
 called ContentPlaceHolder which works as a holder of Content Page. On the
 other hand, each Content Page references a single Master Page and acquires 
 its content. Both the Master Page and Content Page work together to allow 
 developers easier to build a website with a standard appearance.

/////////////////////////////////////////////////////////////////////////////
Code Logical for Basic Master Page:

Step1. Create a Visual Basic ASP.NET Web Application in Visual Studio 2008 
or Visual Web Developer. 

Step2. Right click the project's name and select contect menu item followed 
this order: "Add" -> "New Item..." to open the Add New Item dialog window
and select Master Page, by the name MasterPage.master, to add it to the web
application. 

Step3. Edit the HTML source code of the Master Page according to the sample
in the demo. A background color will make the Master Page easier to be told
from the Content Page which will be added later.

[NOTE] In the HTML source code of the sample page, we will find a web server
control called ContentPlaceHolder. One than one ContentPlaceHolder can be 
located in one page and they work as the holders where we want to locate the 
Content Page.

Step4. Right click the project's name and select contect menu item followed 
this order: "Add" -> "New Item..." to open the Add New Item dialog window
and select Web Content Form and click button "Add" to go on.

Step5. In the new Select Master Page dialog window, choose MasterPage.master.
Then click "OK" to add this Content Page to the website.

[NOTE] A Content Page is much different from a normal ASP.NET web form page.
In the @Page directive, we can find an attribute named MasterPageFile, It is 
to point the Master Page we will use. And also, there is no <body> tag or 
even <html> tag. This is because a Content Page is not a real page. When it 
runs, it is working as a part of the Master Page. So everything in a Content
Page should be located in Content server control.

Step6: Edie the HTML source to add a TextBox and a Button into the Content 
area. Here, we will show how to visit the Master Page from the Content Page.

Step7. Double click the Button to open the Code-Behind page and write these
code to Button.Click event handler:

    Protected Sub Button1_Click(ByVal sender As Object, ByVal e As EventArgs) _
                                Handles Button1.Click
        Dim lbMasterPageHello As Label
        lbMasterPageHello = TryCast(Master.FindControl("lbHello"), Label)

        If lbMasterPageHello IsNot Nothing Then
            lbMasterPageHello.Text = "Hello, " & txtName.Text & "!"
        End If
    End Sub

[NOTE] In Content Page, we can use Me.Master to get an instance of its Master
Page. Once we have the Master Page instance, then we can use Page.FindControl 
method to visit a specific control in the Master Page and edit its property.

[NOTE] Run the application. Enter some texts into the TextBox then click the
Button. Then we can find the texts on the Master Page's Label.


/////////////////////////////////////////////////////////////////////////////
Code Logical for Nested Master Page:

Step1. Right click the project's name and select contect menu item followed
this order: "Add" -> "New Item..." to open the Add New Item dialog window.
Add a Nested Master Page to the application and select MasterPage.master as
the Master Page of this Nested Master Page.

[NOTE] A Nested Master Page is both a Content Page and a Master Page, which 
means, it is embodied in one Master Page and meanwhile, it works as another
Master Page to those Contents which are contained in it.

Step2. Right click the project's name and select contect menu item followed 
this order: "Add" -> "New Item..." to open the Add New Item dialog window.
Add two Web Content Form Pages to the application. ReName them to 
NestedContentPageA.aspx and NestedContentPageB.aspx. In the Select Master 
Page dialog window, choose NestedMasterPage.master as their Master Page.

Step3. Edit the HTML source of the NestedMasterPage.master to contain the 
ContentPlaceHolder that holds the NestedContentPageA and NestedContentPageB.
Also, add two links to to the navigation to these two pages.

[NOTE] Run the application.Switch the links between Content Page and Nested
Content Page. We can find the these two Content Page displayed alternately.
And when it comes to the Nested Content Page, we can also switch between the
NestedContentPageA and NestedContentPageB. All these Content Pages display
according to the choice but leave the Master Page the same. This is way we
use Master Page along with our ASP.NET website that it can help use easier 
to build a website with a standard look.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: ASP.NET Master Pages Overview
http://msdn.microsoft.com/en-us/library/wtxbf3hh.aspx

MSDN: Nested ASP.NET Master Pages
http://msdn.microsoft.com/en-us/library/x2b3ktt7.aspx

MSDN: Create Content Pages for an ASP.NET Master Page
http://msdn.microsoft.com/en-us/library/fft2ye18.aspx

MSDN: How to: Reference ASP.NET Master Page Content
http://msdn.microsoft.com/en-us/library/xxwa0ff0.aspx




/////////////////////////////////////////////////////////////////////////////