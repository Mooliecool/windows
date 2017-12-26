================================================================================
       WINDOWS FORMS APPLICATION : VBWinFormSplashScreen Project Overview
       
                        Splash Screen Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The Splash Screen sample demonstrates how to achieve splash screen effect in 
Windows Forms application.
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:

   1. Create a new form named "SplashScreen1"
   2. Write a method named "Splash1Setting", in the method, set the "FormBorderStyle",
      "BackgroundImage" and "StartPosition" properties as follows:
      
      Private Sub Splash1Setting()
        Me.FormBorderStyle = Windows.Forms.FormBorderStyle.None
        Me.BackgroundImage = VBWinFormSplashScreen.My.Resources.SplashImage
        Me.StartPosition = FormStartPosition.CenterParent
      End Sub
    
      In the New() method, call the Splash1Setting() method.
   3. Right-click the project and choose the "Properties" option, in the "application"
      tab, select the "Splash Screen" as "SplashScreen1".
   4. Built the project.
 
   
/////////////////////////////////////////////////////////////////////////////
References:

1. WindowsFormsApplicationBase.SplashScreen Property 
http://msdn.microsoft.com/en-us/library/microsoft.visualbasic.applicationservices.windowsformsapplicationbase.splashscreen.aspx


/////////////////////////////////////////////////////////////////////////////
