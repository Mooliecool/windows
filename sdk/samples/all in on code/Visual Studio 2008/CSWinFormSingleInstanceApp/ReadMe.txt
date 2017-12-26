================================================================================
       WINDOWS FORMS APPLICATION : CSWinFormSingleInstanceApp Project Overview
       
                        Single Instance Application Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The SingleInstanceApp sample demonstrates how to achieve the goal that only 
one instance of the application is allowed in Windows Forms application. In 
this sample, we are taking advantage of VB.NET Framework since it has built
-in support for single instance application.

/////////////////////////////////////////////////////////////////////////////
Code Logic:


   1. Create a LoginForm which will be used to authenticate the user's 
      indentity.
   2. Create a MainForm which the user will be redirect to after the user 
      has been authenticated.
   3. Create a class inheriting from WindowsFormsApplicationBase, and name
      it "SingleInstanceApp".
   4. Create a class "CSWinFormSingleInstanceApp" which will be specialized 
      used to start the MessageLoop.
   5. If the user has not loggined in the application, and the user tries to 
      start another instance, the LoginForm will be activated.
   6. If the user loggined in the application, and the user tries to start 
      another instance, the MainForm will be activated.
    

/////////////////////////////////////////////////////////////////////////////
References:

   1. .NET MATTERS: Single-Instance apps
	   http://msdn.microsoft.com/en-us/magazine/cc163741.aspx

/////////////////////////////////////////////////////////////////////////////
