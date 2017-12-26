========================================================================
       ASP.NET APPLICATION : VBASPNETFileUpload Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

  The project illustrates how to upload files in an ASP.NET project by use 
  of FileUpload control and VB.NET language. When submit Button is clicked,
  the HasFile property of the FileUpload control is checked to verify that
  a file has been selected to upload. Before the file is saved, File.Exists 
  method is called to check whether a file that has the same name already 
  exists in the path. If so, the name of uploaded file is postfixed with a 
  increasing number so that the exsited file will not be overwritten. After
  the file is uploaded successfully, some basic information is displayed, 
  including the original path, name, the size and the content type of the
  uploaded file. All the information is get from the HttpPostedFile object
  pointed by FileUpload.PostedFile property. 
  

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step1. Create a Visual Basic ASP.NET Web Application named VBASPFileUpload in
Visual Studio 2008 / Visual Web Developer.

Step2. Add a FileUpload control and a Button control into the ASP.NET HTML 
page and rename them according to the following way:

    FileUpload1 -> FileUploader
    Button1     -> btnSubmit

[NOTE] The file in the FileUpload control cannot be automatically saved to
server after a user selects it. Another control, typically like a Button, 
must be provided to submit the page, so that the file can be posted to server. 

Step3: Create a new folder and rename it as UploadFiles, which is used to 
store the uploaded files.

Step4: Double-click the Button in page's Designer View to open the Code-Behind 
page in Visual Basic .NET language.

Step5: Import the namespace System.IO and System.Text at the very beginning
of the page. Or you can use the complete function name like System.IO.Path in
the code.

Step6: Create SaveUploadFile() function which calls HttpPostedFile.SaveAs() 
method to save the file to the server. Actually, FileUpload.SaveAs() method 
does the same thing that calling HttpPostedFile.SaveAs() method itself.

    Protected Function SaveUploadFile(ByVal uploadedFile As HttpPostedFile, _
                                      ByVal SavePath As String) As Boolean
        '...
        uploadedFile.SaveAs(Path.Combine(SavePath, fileName))
        '...
    End Function

[NOTE] When SaveAs() method is called, the full path of the directory on the 
server must be specified, in which the file to upload will be saved. If not, 
an HttpException exception is thrown. This behavior helps to keep the files 
on the server secure, by preventing users from being able to write uploaded 
files to arbitrary locations to server, like some sensitive root directories. 

Step7: Create GetUploadFileInfo() funtion to get the basic information of 
the uploaded file.

    Protected Function GetUploadFileInfo(ByVal uploadedFile As HttpPostedFile) _
                                         As String
        '...
    End Function

Step8: Eidt the Button's click event handler to call SaveUploadFile() to save
the file. After uploading, call GetUploadFileInfo() function to show file's 
information. 

[NOTE] Before calling SaveUploadFile() function, FileUpload.HasFile property
need to be checked to verify the FileUpload control contains a file. If it 
returns false, display a message to tell the user no file has been selected 
to upload. Do not check the PostedFile.Length property to validate whether a    
file to upload exists because this property could contain 0 bytes even when 
FileUpload control is blank. As a result, the PostedFile property can return 
a non-null value. 


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: FileUpload Class
http://msdn.microsoft.com/en-us/library/system.web.ui.webcontrols.fileupload.aspx

MSDN: Uploading Files in ASP.NET 2.0
http://msdn.microsoft.com/en-us/library/aa479405.aspx


/////////////////////////////////////////////////////////////////////////////