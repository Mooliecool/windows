========================================================================
    CONSOLE APPLICATION : VBPowerShell Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

This sample indicates how to call Powershell from VB.NET language. It first
creats a Runspace object in System.Management.Automation namespace. Then 
it creats a Pipeline from Runspace. The Pipeline is used to host a line of
commands which are supposed to be executed. The example call Get-Process 
command to get all processes whose name are started with "V".


/////////////////////////////////////////////////////////////////////////////
Creation:

1.To create this project, we first need to install PowerShell. We can find 
the download link from the following KB article,
http://support.microsoft.com/kb/968929

2.Create a Console application and reference the System.Management.Automation
assembly from the following location,
C:\Program Files\Reference Assemblies\Microsoft\WindowsPowerShell\

3.Write codes,
1).We first create a runspace which is used to host the Powershell script 
execution.
2).From the Runsapce, we can create a Pipeline. The pipeline can be used to host
commands that we want to execute.
3).We construct a command, add parameters to the command.Parameters property.
Then we add it to the Pipeline.
4).Call Pipeline.Invoke to execute all commands in the pipeline.
5).We can get the results as a Collection of PSObject type. Loop the collection
to access each of the object.
6).Note we can access the .NET system object by PSObject.BaseObject property.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Runspace Class
http://msdn.microsoft.com/en-us/library/system.management.automation.runspaces.runspace(VS.85).aspx

MSDN: Pipeline.Invoke Method ()
http://msdn.microsoft.com/en-us/library/ms569128(VS.85).aspx

How to call Powershell Script function from C# ? 
http://social.msdn.microsoft.com/Forums/en-AU/csharpgeneral/thread/faa70c95-6191-4f64-bb5a-5b67b8453237


/////////////////////////////////////////////////////////////////////////////