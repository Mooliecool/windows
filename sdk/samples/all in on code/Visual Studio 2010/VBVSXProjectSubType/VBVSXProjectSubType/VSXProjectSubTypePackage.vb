Imports System.Globalization
Imports System.Runtime.InteropServices
Imports Microsoft.VisualStudio.Shell

''' <summary>
''' This is the class that implements the package exposed by this assembly.
'''
''' The minimum requirement for a class to be considered a valid package for Visual Studio
''' is to implement the IVsPackage interface and register itself with the shell.
''' This package uses the helper classes defined inside the Managed Package Framework (MPF)
''' to do it: it derives from the Package class that provides the implementation of the 
''' IVsPackage interface and uses the registration attributes defined in the framework to 
''' register itself and its components with the shell.
''' </summary>
' The PackageRegistration attribute tells the PkgDef creation utility (CreatePkgDef.exe) that this class
' is a package.
'
' The InstalledProductRegistration attribute is used to register the information needed to show this package
' in the Help/About dialog of Visual Studio.

<PackageRegistration(UseManagedResourcesOnly:=True), _
ProvideObject(GetType(ProjectFlavor.CustomPropertyPage),
    RegisterUsing:=RegistrationMethod.CodeBase), _
ProvideProjectFactory(GetType(ProjectFlavor.CustomPropertyPageProjectFactory),
    "Task Project",
    Nothing,
    Nothing,
    Nothing,
    "..\Templates\Projects"), _
Guid(GuidList.guidVBVSXProjectSubTypePkgString)>
Public NotInheritable Class VSXProjectSubTypePackage
    Inherits Package

    ''' <summary>
    ''' Default constructor of the package.
    ''' Inside this method you can place any initialization code that does not require 
    ''' any Visual Studio service because at this point the package object is created but 
    ''' not sited yet inside Visual Studio environment. The place to do all the other 
    ''' initialization is the Initialize method.
    ''' </summary>
    Public Sub New()
        Trace.WriteLine(String.Format(CultureInfo.CurrentCulture,
                                      "Entering constructor for: {0}",
                                      Me.GetType().Name))
    End Sub



    ''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
    ' Overriden Package Implementation
#Region "Package Members"

    ''' <summary>
    ''' Initialization of the package; this method is called right after the package is sited, so this is the place
    ''' where you can put all the initilaization code that rely on services provided by VisualStudio.
    ''' </summary>
    Protected Overrides Sub Initialize()
        Trace.WriteLine(String.Format(CultureInfo.CurrentCulture,
                                      "Entering Initialize() of: {0}",
                                      Me.GetType().Name))
        MyBase.Initialize()
        Me.RegisterProjectFactory(New ProjectFlavor.CustomPropertyPageProjectFactory(Me))
    End Sub
#End Region

    ''' <summary>
    ''' Allow a component such as project, factory, toolwindow,... to
    ''' get access to VS services.
    ''' </summary>
    Friend Function GetVsService(ByVal type As Type) As Object
        Return Me.GetService(type)
    End Function

End Class
