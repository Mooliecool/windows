========================================================================
         ASP.NET APPLICATION : CSASPNETMVCDataView Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

  The project illustrates how to use DataViews in MVC to handle the data 
proccessing work.



/////////////////////////////////////////////////////////////////////////////
Prerequisite:

MVC2.0 for Visual Studio 2008 is required and it's avaiable in the following
link:

http://www.microsoft.com/downloads/en/details.aspx?FamilyID=c9ba1fe1-3ba8-439a-9e21-def90a8615a9&displaylang=en


/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step1. Make sure that you have installed MVC2.0 for Visual Studio 2008.

Step2. Create a C# ASP.NET MVC 2 Web Application in Visual Studio 
2008 and name it as CSASPNETMVCDataView.


Step3. Create AccountController.cs,HomeController.cs,PersonalController.cs 
files in Controllers folder.

Step4. Create AccountModels.cs,Person.cs files in Models folder.      

Step5. Add Account,Home,Personal,Shared sub-folders in Views folder, 
and create the corresponding files accroding to the sample project.

Step6. Create the folders and files accroding to the sample project, 
including the scripts files in Script folder.

Step7. Create the logics for models and controllers according to 
the sample project.

[NOTE] For views pages, the mapping model register info will be added
automatically.See:

Inherits="System.Web.Mvc.ViewPage<CSASPNETMVCDataView.Models.ChangePasswordModel"

For more details of how to process data in MVC2.0, see:

http://www.asp.net/mvc/tutorials/creating-model-classes-with-the-entity-framework-cs

Step8: Add the following logic in AccountModels.cs file in Models folder.

  public class ChangePasswordModel
    {
        [Required]
        [DataType(DataType.Password)]
        [DisplayName("Current password")]
        public string OldPassword { get; set; }

        [Required]
        [ValidatePasswordLength]
        [DataType(DataType.Password)]
        [DisplayName("New password")]
        public string NewPassword { get; set; }

        [Required]
        [DataType(DataType.Password)]
        [DisplayName("Confirm new password")]
        public string ConfirmPassword { get; set; }
    }


Step9: Add other codes to corresponding files according to the sample.

Step10: Now, you can run the page to see the achievement we did before :)

/////////////////////////////////////////////////////////////////////////////
References:

MVC Tutorials:
http://www.asp.net/mvc/tutorials