================================================================================
       WINDOWS FORMS APPLICATION : CSWinFormSplashScreen Project Overview
       
                        Splash Screen Sample
                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The Splash Screen sample demonstrates how to achieve splash screen effect in 
Windows Forms application.
   

/////////////////////////////////////////////////////////////////////////////
Code Logic:

Solution 1: Customized SplashScreen with "fade in" and "fade out" effect.

   1. Run a splash screen form whose original opacity is 0.5.
   2. Use a timer to increase the opacity of the splash screen to 1.0, and then 
      decrease to 0.
   3. Close the splash screen,and stop the timer.
   4. Start the main form.
   
Solution 2: Using VB.NET Framework without "fade in" and "fade out" effect.

   1. Add reference to "Microsoft.VisualBasic".
   2. Create a class inheriting from WindowsFormsApplicationBase.
   3. Override OnCreateSplashScreen method, and assign the SplashScreen property 
      to an instance of the Splash form instance.
   4. Override OnCreateMainForm method, and assign the MainForm property to an 
      instance of the main form instance.(We can keep the Splash screen for a 
      while before set the MainForm property).
   5. In the static Main method, we can use the class deriving from
      WindowsFormsApplicationBase to run the application.
 
   
/////////////////////////////////////////////////////////////////////////////
References:

1. WindowsFormsApplicationBase.SplashScreen Property 
http://msdn.microsoft.com/en-us/library/microsoft.visualbasic.applicationservices.windowsformsapplicationbase.splashscreen.aspx


/////////////////////////////////////////////////////////////////////////////
