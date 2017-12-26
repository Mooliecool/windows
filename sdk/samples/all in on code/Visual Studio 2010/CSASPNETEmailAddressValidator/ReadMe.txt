========================================================================
            CSASPNETEmailAddressValidator Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The project illustrates how to send a confirmation Email to check whether an
Email address is available.

/////////////////////////////////////////////////////////////////////////////
Demo the Sample. 

Please follow these demonstration steps below.

Step 1: Open the CSASPNETEmailAddressValidator.sln.

Step 2: Expand the CSASPNETEmailAddressValidator website and press 
        Ctrl + F5 to show the Default.aspx.

Step 3: We will see a Wizard. In the first step, input a SMTP server name or
        IP address and one Email address based on that SMTP server and its
		password. We need to use this to send the confirmation Email.
		In this sample, we use Hotmail, so if you have a hotmail, you can
		input your hotmail address and password and then click Next.

Step 4: In the second step, input an Email address which need to be validated.
        Of course, you need to see the message to process the next step.

Step 5: Open the mail message which you have received. Click the link or copy the 
        link address to the browser and you can see a 'Congratulation' message.

Step 6: Validation finished.

/////////////////////////////////////////////////////////////////////////////
Code Logical:

Step 1.  Create a C# "ASP.NET Empty Web Application" in Visual Studio 2010 or
         Visual Web Developer 2010. Name it as "CSASPNETEmailAddressValidator".

Step 2.  Add two folders, "Handler" and "Module".

Step 3.  Add a new database file in the App_Data and create a new table called,
         "tblEmailValidation". Add six columns which are illustrated like below:
		 id: the identity key of the table;
		 EmailAddress: store the Email address which need to be validated;
		 IsValidated: store a bit to check whether the validation is completed.
		 IsSendCheckEmail: store a bit to check whether the message has been sent.
		 ValidatingStartTime: store the datetime which start the validation.
		 ValdateKey: a unique key which used to tell different validation links.

Step 4.  Add a new Linq to SQL class in the Module folder and name it as 
         EmailAddressValidation.dbml. Open the Server Explorer, create a connection
		 to the EmailValidationDB.mdf and drag the tblEmailValidation table and
		 drop it to the desktop of the EmailAddressValidation.dbml. Then build the
		 project.

Step 5.  Create a new class file, name it as EmailValidation.cs. Write the codes
         like the sample and we can find more details from the comments 
		 in the sample file.
         

Step 6.  Create a new HttpHandler in the Handler folder. Write the codes like the
         sample and we can find more details from the comments in the sample file.
         
         
Step 7.  Modify the Webform1.aspx to Default.aspx and create a Wizard control there.
         Follow the sample to complete the markups.
         
Step 8.  Open the Default.aspx.cs. Write the codes like the sample. You can get more
         details from the comments in the sample file.

Step 9.  Open the web.config. Add a new HttpHandler like below in the <system.web> 
         node.
		 [Code]
		 <httpHandlers>
             <add path="mail.axd" verb="GET" validate="false" type="CSASPNETEmailAddresseValidator.Handler.EmailAvailableValidationHandler" />
         </httpHandlers>
         [/Code]  

Step 10. Build the application and you can debug it.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: SmtpClient Class
http://msdn.microsoft.com/en-us/library/system.net.mail.smtpclient.aspx

MSDN: SQL-CLR Type Mapping (LINQ to SQL)
http://msdn.microsoft.com/en-us/library/bb386947.aspx

MSDN: HttpHandlers
http://msdn.microsoft.com/en-us/library/5c67a8bd(VS.71).aspx

/////////////////////////////////////////////////////////////////////////////