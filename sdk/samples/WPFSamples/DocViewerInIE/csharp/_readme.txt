DocViewerInIE SDK Sample - _ReadMe_.txt
Copyright (c) Microsoft Corporation. All rights reserved.

-------------------------------------------------------------------------------
DocViewerInIE Operating Notes:

1. The first time that the project is executed for debug a dialog may appear
   stating that there is no debug information available for PresentationHost.exe.
   This dialog can be safely dismissed, simply click OK to continue.

2. XAML Browser Applications (XBAPs) must be digitally signed.  The DocViewerInIE
   sample provides a temporary key set with a blank password.  If Visual Studio
   prompts for a password, simply click "OK" to continue.

   (The temporary key is contained in the file DocViewerInIE_TemporaryKey.pfx)

-------------------------------------------------------------------------------
XAML Browser Application Notes:

1. XAML Browser Applications (XBAPs) run in a protected sandbox with limited
   "Internet Zone" permissions".  Only those .NET Framework 3.0 features
   validated as safe by the Microsoft.NET security team can run in the sandbox.

2. Since XBAP applications execute in the Internet Zone with restricted
   permissions, normal .NET Framework "Window" operations do not work by design
   (XBAP applications do not have permissions to "pop-up" new windows).

-------------------------------------------------------------------------------
XAML Browser Application Debugging - (F5)

The "Enable unmanaged code debugging" debug option* must be set to debug a
XAML Browser Application in Visual Studio.  To do this:

1. Start Visual Studio and open the DocViewerInIE.csproj project file.

2. On the Visual Studio menu, click "Project", click "DocViewerInIE Properties",
   and then click the "Debug" tab on the DocViewerInIE properties page.

3. On the "Debug" tab under "Enable Debuggers", set the "Enable unmanaged code
   debugging" check box.
   
*Developers using Visual C# and Visual Basic Express do not need to do this.

[end]
