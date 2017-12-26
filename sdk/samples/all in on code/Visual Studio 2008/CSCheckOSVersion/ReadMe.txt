========================================================================
    CONSOLE APPLICATION : CSCheckOSVersion Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Use:

The CSCheckOSVersion sample demonstrates how to detect the version of the 
current operating system, and how to make application that checks for the 
minimum operating system version work with later operating system versions.

Note that compatibility mode set in the executable program's property dialog 
does not apply to the managed code applications. However, if your application 
is mixed (native and managed) and you are checking for the operating system 
version with a native call, then setting compatibility mode may help. You may 
refer to the CppCheckOSVersion for more information about compatibility mode.

The most common application compatibility issue that users as well as 
developers face is when an application fails upon checking the operating 
system version. A lot can go wrong when version checking is misused. A user 
might experience a silent fail where the application simply fails to load and 
nothing happens. Or, a user might see a dialog box indicating something to 
the effect of -you must be running Microsoft Windows XP or later- when in 
fact, the computer is running Windows 7. Many other consequences to poor 
version checking can inconvenience users as well. 

When an application runs on an "incompatible" (due to poor version checking) 
version of Windows, it will generally display an error message, but it may 
also exit silently or behave erratically. Often, if we work around the 
version checking, the application will run well. End-users and IT 
professionals may apply a fix to let the application think it is running on 
an older version of Windows.


/////////////////////////////////////////////////////////////////////////////
Code Logic:

1. Detect the current operating system version: 

	Environment.OSVersion.Version
	Environment.OSVersion.VersionString

2. Check if the current OS is at least Windows XP

	if (Environment.OSVersion.Version < new Version(5, 1))
	{
		// Quit the application due to incompatible OS
	}


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Environment.OSVersion Property 
http://msdn.microsoft.com/en-us/library/system.environment.osversion.aspx


/////////////////////////////////////////////////////////////////////////////