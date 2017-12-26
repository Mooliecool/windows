=============================================================================
        APPLICATION : CSCreateLowIntegrityProcess Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary: 

The code sample demonstrates how to start a low-integrity process. The 
application launches itself at the low integrity level when you click the 
"Launch myself at low integrity level" button on the application. Low 
integrity processes can only write to low integrity locations, such as the 
%USERPROFILE%\AppData\LocalLow folder or the HKEY_CURRENT_USER\Software\
AppDataLow key. If you attempt to gain write access to objects at a higher 
integrity levels, you will get an access denied error even though the 
user's SID is granted write access in the discretionary access control list 
(DACL). 

By default, child processes inherit the integrity level of their parent 
process. To start a low-integrity process, you must start a new child process
with a low-integrity access token by using CreateProcessAsUser. Please refer 
to the CreateLowIntegrityProcess sample function for details.


/////////////////////////////////////////////////////////////////////////////
Prerequisite:

You must run this sample on Windows Vista or newer operating systems.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the low-integrity process 
sample.

Step1. After you successfully build the sample project in Visual Studio 2008, 
you will get the application: CSCreateLowIntegrityProcess.exe. 

Step2. Run the application as an ordinary user on a Windows Vista or Windows 
7 system with UAC fully enabled. The application should display the following
content on the main dialog.

  Current Integrity Level: Medium
  [Launch myself at low integrity level]
  
  Tests:
  [Write to the LocalAppData folder]
  [Write to the LocalAppDataLow folder]

The process is running at the Medium integrity level. If you click on the 
[Write to the LocalAppData folder] button or the [Write to the 
LocalAppDataLow folder] button, you will get a message box saying that the 
file write operation succeeds.

Step3. Click on the [Launch myself at low integrity level] button. A new 
instance of the CSCreateLowIntegrityProcess application is launched, but 
this time, it is running at the Low integrity level.

  Current Integrity Level: Low
  [Launch myself at low integrity level]
  
  Tests:
  [Write to the LocalAppData folder]
  [Write to the LocalAppDataLow folder]

If you click on the [Write to the LocalAppData folder] button, you will get 
the 0x80070005 error (Access Denied) with the following message.

  Access to the path '%USERPROFILE%\AppData\Local\testfile.txt' is denied.

This indicates that the low-integrity process is denied to write to the 
LocalAppData known folder (%USERPROFILE%\AppData\Local\) because the folder 
has a higher (Medium) integrity level:

  accesschk "%USERPROFILE%\AppData\Local" -d
  %USERPROFILE%\AppData\Local
    Medium Mandatory Level (Default) [No-Write-Up]
    RW NT AUTHORITY\SYSTEM
    RW BUILTIN\Administrators
    RW [MachineName]\[UserName]

However, the low-integrity process is able to write to the LocalAppDataLow 
known folder (%USERPROFILE%\AppData\LocalLow\) when you click the [Write to 
the LocalAppDataLow folder] button, because the folder has the same Low 
integrity level:

  accesschk "%USERPROFILE%\AppData\LocalLow" -d
  %USERPROFILE%\AppData\LocalLow
    Low Mandatory Level [No-Write-Up]
    RW NT AUTHORITY\SYSTEM
    RW BUILTIN\Administrators
    RW [MachineName]\[UserName]

Step4. Close the application and clean up the test files (testfile.txt) 
created by the application in %USERPROFILE%\AppData\Local\ and 
%USERPROFILE%\AppData\LocalLow\.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

A. Starting a process at low integrity

By default, child processes inherit the integrity level of their parent 
process. To start a low-integrity process, you must start a new child process 
with a low-integrity access token using the function CreateProcessAsUser.

To start a low-integrity process 

  1) Duplicate the handle of the current process, which is at medium 
     integrity level by P/Invoking OpenProcessToken and DuplicateTokenEx.

  2) P/Invoke SetTokenInformation to set the integrity level in the access 
     token to Low.

  3) P/Invoke CreateProcessAsUser to create a new process using the handle to 
     the low integrity access token.

CreateProcessAsUser updates the security descriptor in the new child process 
and the security descriptor for the access token to match the integrity level 
of the low-integrity access token. The CreateLowIntegrityProcess function in 
the code sample demonstrates this process.

B. Detecting the integrity level of the current process

The GetProcessIntegrityLevel function in the code sample demonstrates how to 
get the integrity level of the current process. 

  1) Open the primary access token of the process with TOKEN_QUERY by 
     P/Invoking OpenProcessToken.
  
  2) P/Invoke GetTokenInformation to query the TokenIntegrityLevel 
     information from the primary access token.


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Designing Applications to Run at a Low Integrity Level
http://msdn.microsoft.com/en-us/library/bb625960.aspx

MSDN: Understanding and Working in Protected Mode Internet Explorer
http://msdn.microsoft.com/en-us/library/bb250462(VS.85).aspx


/////////////////////////////////////////////////////////////////////////////