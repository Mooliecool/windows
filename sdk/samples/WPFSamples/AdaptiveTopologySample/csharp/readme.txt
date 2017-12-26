XAML Browser Applications Readme
----------------------------------
XAML Browser Applications run in a sandbox with "Internet Permissions".
Only those WinFX features that have been successfully security reviewed and validated as safe by the WPF team will run inside the sandbox.

WPF Item Templates - which work in the sandbox?
--------------------------------------------------------
Won't work by design - WPF Window - In the internet zone, you don't have the permission to "popup" new windows.

Debugging XAML Browser Applications - (F5)
--------------------------------------------------------
In order to successfully debug an XAML Browser Applications in Visual Studio, you must enable unmanaged code debugging
via the Debug page in the Properties view.

Developers using Visual C# and Visual Basic Express do not need to do this - however, they do
need to save any newly created XAML Browser Applications project.  Close the Solution.  Then open
it, or F5 will not work.

The first time that this project is debugged, a dialog may appear stating that there 
is no debug information available for PresentationHost.exe.  This dialog can be
safely dismissed.
