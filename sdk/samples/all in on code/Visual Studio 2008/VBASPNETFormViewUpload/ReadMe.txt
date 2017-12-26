========================================================================
    ASP.NET APPLICATION : VBASPNETFormViewUpload Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This VBASPNETFormViewUpload sample demonstrates how to display and upload 
images in an ASP.NET FormView control and how to implement Insert, Edit, 
Update, Delete and Paging functions in the control. 

This project includes two pages: Default and Image.

Default populates a FormView control with data from a SQL Server database 
and provides UI for data manipulation.

Image is used to retrieve the image from a SQL Server database and display 
it in the Web page.


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Create a VB ASP.NET Web Application named VBASPNETFormViewUpload in 
Visual Studio 2008 / Visual Web Developer.


Step2. Drag a FormView control into the Default page.

    (1) Rename the FormView to fvPerson.
     
    (2) In the Source view, copy and paste the markup of following three 
    templates from the sample:

    ItemTemplate: render the particular record displayed in the FormView.
    EditItemTemplate: customize the editing interface for the FormView.
    InsertItemTemplate: customize the inserting interface for the FormView. 

    Related references:
    
    ASP.NET: Using the FormView's Templates	
    MSDN: FormView Class
    MSDN: Image Class
        
    
Step3. Copy the following methods from the sample,and paste them in the 
code-behind of Default page:

    Page_Load Method:
    Initialize underlying objects, when the Page is accessed 
    for the first time.

    BindFormView Method:
    Bind the FormView control with data from a SQL Server table.

    Related reference:
    
    MSDN: Using Statement (Visual Basic)
        
    
Step4. Navigate to the Property panel of fvPerson and then switch to Event. 
Double-click on the following events to generate the Event handlers. 
After that, fill the generated methods with the sample code.

    (1)	ModeChanging Event: Occurs when the FormView control switches 
    between edit, insert, and read-only mode, but before the mode changes.
    
    In this event, we need to switch FormView control to the new mode and 
    then rebind the FormView control to show data in new mode.	
    
    ////////////////////////////////////////////////////////////////
    fvPerson.ChangeMode(e.NewMode)
    BindFormView()
    ////////////////////////////////////////////////////////////////

    Related reference:
    
    MSDN: FormView.ModeChanging	

    (2)	PageIndexChanging Event: Occurs when a pager button within the 
    control is clicked.
    
    In order to show data in the new page, we need to set the index of new 
    page and then rebind the FormView control. 	

    ////////////////////////////////////////////////////////////////    
    fvPerson.PageIndex = e.NewPageIndex
    BindFormView()
    ////////////////////////////////////////////////////////////////
    
    Related reference:	
    
    MSDN: FormView.PageIndexChanging Event


    (3)	ItemInserting Event: Occurs when an Insert button within a FormView 
    control is clicked, but before the insert operation.
    
    After clicking Insert button, we need to get the first name, last name 
    and specified image file (bytes) from the 	InsertItemTemplate of the 
    FormView control.
    
    ////////////////////////////////////////////////////////////////
    Dim strLastName As String = DirectCast(fvPerson.Row.FindControl("tbLastName"), TextBox).Text
    Dim strFirstName As String = DirectCast(fvPerson.Row.FindControl("tbFirstName"), TextBox).Text

    cmd.Parameters.Add("@LastName", SqlDbType.NVarChar, 50).Value = strLastName
    cmd.Parameters.Add("@FirstName", SqlDbType.NVarChar, 50).Value = strFirstName

    Dim uploadPicture As FileUpload = DirectCast(fvPerson.FindControl("uploadPicture"), FileUpload)

    If uploadPicture.HasFile Then
        cmd.Parameters.Add("@Picture", SqlDbType.VarBinary).Value = uploadPicture.FileBytes
    Else
        cmd.Parameters.Add("@Picture", SqlDbType.VarBinary).Value = DBNull.Value
    End If
    ////////////////////////////////////////////////////////////////
                

    Related reference:	
    
    MSDN: FormView.ItemInserting Event	

    (4)	ItemUpdating Event: Occurs when an Update button within a FormView 
    control is clicked, but before the update operation.
    
    After clicking Update button, we need to get and pass the person ID, 
    first name, last name and specified image file (bytes) from the 
    EditItemTemplate of the FormView control.	

    //////////////////////////////////////////////////////////////// 
    Dim strPersonID As String = DirectCast(fvPerson.Row.FindControl("lblPersonID"), Label).Text
    ////////////////////////////////////////////////////////////////

    Related reference:	
    MSDN: FormView.ItemUpdating Event

    (5)	ItemDeletingEvent: Occurs when a Delete button within a FormView 
    control is clicked, but before the delete operation.
    
    We get the PersonID from the ItemTemplate of the FormView control and 
    then delete the person record in the database based on the PersonID.
    
    ////////////////////////////////////////////////////////////////
    Dim strPersonID As String = DirectCast(fvPerson.Row.FindControl("lblPersonID"), Label).Text
    ////////////////////////////////////////////////////////////////
    
    Related reference:	
    
    MSDN: FormView.ItemDeleting Event


Step5. Create a new Web page named Image in the project. Copy the Page_Load 
method from the sample, and paste it in code-behind of Image page:

In this page, we retrieve the image data from the database, convert it to a 
bytes array and then write the array to the HTTP output stream 
to display the image.

    //////////////////////////////////////////////////////////////// 
    Dim bytes As Byte() = DirectCast(cmd.ExecuteScalar(), Byte())

    If bytes IsNot Nothing Then
        Response.ContentType = "image/jpeg"
        Response.BinaryWrite(bytes)
        Response.End()
    End If
    //////////////////////////////////////////////////////////////// 

Related references:

MSDN: Request Object
MSDN: Response Object


/////////////////////////////////////////////////////////////////////////////
References:

ASP.NET: Using the FormView's Templates
http://www.asp.net/learn/data-access/tutorial-14-vb.aspx

MSDN: Image Class
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.image.aspx

MSDN: FormView Class
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.formview.aspx

MSDN: Using Statement (Visual Basic)
http://msdn.microsoft.com/en-us/library/htd05whh.aspx

MSDN: FormView.ModeChanging
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.formview.modechanging.aspx

MSDN: FormView.PageIndexChanging Event
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.formview.pageindexchanging.aspx

MSDN: FormView.ItemInserting Event
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.formview.iteminserting.aspx

MSDN: FormView.ItemUpdating Event
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.formview.itemupdating.aspx

MSDN: FormView.ItemDeleting Event
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.formview.itemdeleting.aspx

MSDN: Request Object
http://msdn.microsoft.com/en-us/library/ms524948.aspx

MSDN: Response Object
http://msdn.microsoft.com/en-us/library/ms525405.aspx


/////////////////////////////////////////////////////////////////////////////