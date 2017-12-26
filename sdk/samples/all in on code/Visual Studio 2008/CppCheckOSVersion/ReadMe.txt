========================================================================
    CONSOLE APPLICATION : CppCheckOSVersion Project Overview
========================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The CppCheckOSVersion demonstrates how to detect the current OS version, and 
how to make application that checks for the minimum operating system version 
work with later operating system versions.

We call GetVersionEx to detect the current OS version. If compatibility mode 
is in effect, the GetVersionEx function reports the operating system as it 
identifies itself, which may not be the operating system that is installed. 
For example, if compatibility mode is in effect, GetVersionEx reports the 
operating system that is selected for application compatibility.

To compare the current system version to a required version, use the 
VerifyVersionInfo function instead of using GetVersionEx to perform the 
comparison yourself. Please note that compatibility mode does not affect the 
result of VerifyVersionInfo. VerifyVersionInfo always reports the comparison 
result based on the operating system that is installed.

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

	OSVERSIONINFOEX osVersionInfo;
	ZeroMemory(&osVersionInfo, sizeof(OSVERSIONINFOEX));
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	GetVersionEx((LPOSVERSIONINFO) &osVersionInfo);

2. Check if the current OS is at least Windows XP

	OSVERSIONINFOEX osVersionInfoToCompare;
	ZeroMemory(&osVersionInfoToCompare, sizeof(OSVERSIONINFOEX));
	osVersionInfoToCompare.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osVersionInfoToCompare.dwMajorVersion = 5;
	osVersionInfoToCompare.dwMinorVersion = 1;		// Windows XP
	osVersionInfoToCompare.wServicePackMajor = 2;	// Service Pack 2
	osVersionInfoToCompare.wServicePackMinor = 0;
	
	ULONGLONG comparisonInfo = 0;
	BYTE conditionMask = VER_GREATER_EQUAL;
	VER_SET_CONDITION(comparisonInfo, VER_MAJORVERSION, conditionMask);
	VER_SET_CONDITION(comparisonInfo, VER_MINORVERSION, conditionMask);
	VER_SET_CONDITION(comparisonInfo, VER_SERVICEPACKMAJOR, conditionMask);
	VER_SET_CONDITION(comparisonInfo, VER_SERVICEPACKMINOR, conditionMask);
	
	if (!VerifyVersionInfo(&osVersionInfoToCompare, VER_MAJORVERSION | 
		VER_MINORVERSION | VER_SERVICEPACKMAJOR | VER_SERVICEPACKMINOR,
		comparisonInfo))
	{
		// Quit the application due to incompatible OS
	}


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: GetVersionEx Function
http://msdn.microsoft.com/en-us/library/ms724451(VS.85).aspx

MSDN: Getting the System Version
http://msdn.microsoft.com/en-us/library/ms724429(VS.85).aspx

MSDN: VerifyVersionInfo Function
http://msdn.microsoft.com/en-us/library/ms725492(VS.85).aspx

MSDN: Verifying the System Version
http://msdn.microsoft.com/en-us/library/ms725491(VS.85).aspx


/////////////////////////////////////////////////////////////////////////////