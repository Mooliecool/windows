================================================================================
       VSX application : CSVSPackageMonitorFileChange Project Overview                        
===============================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

Visual Studio provides SVsFileChangeEx service enables arbitrary components 
to register to be notified when a file is modified outside of the Environment.

This service is useful when you are performing some operation which will be 
interupted by file changes from outside environment.

The service is similar with FileSystemWatcher Class.
http://msdn.microsoft.com/en-us/library/system.io.filesystemwatcher.aspx

To use this sample, follow the steps as below:

1. Start experimental VS instance
2. The package will be autoloaded automatically when there is no any solution 
loaded.
3. The demo monitors user's desktop directory, so please do file or directory 
changes in desktop.
4. Visual Studio will popup window whenever a change is made.


//////////////////////////////////////////////////////////////////////////////
Prerequisites:

VS 2008 SDK must be installed on the machine. You can download it from:
http://www.microsoft.com/downloads/details.aspx?FamilyID=30402623-93ca-479a-867c-04dc45164f5b&displaylang=en

Otherwise the project may not be opened by Visual Studio.

If you run this project on a x64 OS, please also config the Debug tab of the project
Setting. Set the "Start external program" to 
C:\Program Files(x86)\Microsoft Visual Studio 9.0\Common7\IDE\devenv.exe

NOTE: The Package Load Failure Dialog occurs because there is no PLK(Package Load Key)
      Specified in this package. To obtain a PLK, please to go to WebSite:
      http://msdn.microsoft.com/en-us/vsx/cc655795.aspx
      More info
      http://msdn.microsoft.com/en-us/library/bb165395.aspx

/////////////////////////////////////////////////////////////////////////////

Steps:
In order to implement this sample, following are the core steps:
(For detailed informaiton, please view sample code)

1. Create package class and specify AutoLoad attribute:
[ProvideAutoLoad("{adfc4e64-0397-11d1-9f4e-00a0c911004f}")]
adfc4e64-0397-11d1-9f4e-00a0c911004f represents context that there is 
no solution is loaded.

2. In the package's initializate method, add following code:

IVsFileChangeEx fileChangeService =
    GetService(typeof(SVsFileChangeEx)) as IVsFileChangeEx;
monitor = new CSVSMonitorFileChange();
uint cookie;

// Enables a client to receive notifications of changes to a directory.
fileChangeService.AdviseDirChange(

    // String form of the moniker identifier of 
    // the directory in the project system.
    Environment.GetFolderPath(
        Environment.SpecialFolder.Desktop),

    // If true, then events should also be fired 
    // for changes to sub directories. If false, 
    // then events should not be fired for changes 
    // to sub directories.
    Convert.ToInt32(true),

    // IVsFileChangeEvents Interface on the object 
    // requesting notification of file change events.
    monitor,

    // Unique identifier for the file that is 
    // associated with the event sink.
    out cookie
);

3. Implements CSVSMonitorFileChange class which inherited from IVsFileChangeEvents
Implements DirectoryChanged and FilesChanged methods to popup message box
when a change is monitored.

/////////////////////////////////////////////////////////////////////////////
References:

How to get notifications in editor when the file is modified outside of the editor?
http://blogs.msdn.com/dr._ex/archive/2005/11/01/487721.aspx

IVsFileChangeEvents Interface
http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.interop.ivsfilechangeevents.aspx

SVsFileChangeEx Interface
http://msdn.microsoft.com/en-us/library/microsoft.visualstudio.shell.interop.svsfilechangeex(VS.80).aspx

FileSystemWatcher Class
http://msdn.microsoft.com/en-us/library/system.io.filesystemwatcher.aspx



