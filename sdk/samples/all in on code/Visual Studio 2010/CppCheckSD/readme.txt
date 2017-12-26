=============================================================================
	   Windows Console APPLICATION: CppCheckSD Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The sample demonstrates how to obtain and display the security descriptor for
various Kernel Objects in Windows.  The sample by default only obtains the 
Access Allowed Aces (DACLs) and NOT the System Audit Acess (SACLs) although
Integrity Aces are SACLs so these will be displayed as well.  By default
the Security Descriptor is displayed in Security Descriptor Definition
Language (SDDL) format.  There is a switch to get more detailed information 
on the security descriptor.

You can obtain the security descriptor for the following kernel objects:

    * mailslot
    * service control manager
    * directory 
    * event
    * file
    * thread
    * memory mapped file
    * job object
    * desktop
    * printer
    * mutex
    * named pipe
    * process access token
    * process
    * registry key
    * sempahore
    * network share
    * service
    * window station


////////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Build the sample project in Visual Studio 2010.

Step2. Start a cmd.exe window and Run CppCheckSD.exe. This application will show 
the following help text.

    Usage: CppCheckSD [object] [name] </v> </a>
     /v : VERBOSE
     /a : Show Audit Aces

     -a : mailslot, use \\[server]\mailslot\[mailslotname]
     -c : service control manager
     -d : directory or driver letter, use \\.\[driveletter]
     -e : event
     -f : file
     -h : thread, use tid instead of name
     -i : memory mapped file
     -j : job object
     -k : desktop, use [window station\desktop]
     -l : printer, use \\[server]\[printername]
     -m : mutex
     -n : named pipe, use \\[server or .]\pipe\[pipename]
     -o : process access token, use pid instead of name
     -p : process, use pid instead of name
     -r : registry key, use CLASSES_ROOT, CURRENT_USER, MACHINE, or USERS suchas MACHINE\Software
     -s : sempahore
     -t : network share, use [\\server\sharename]
     -v : service, use [\\server\service]
     -w : window station
     -x : 32 bit registry key
     -y : kernel transaction
     -z : waitable timer

Step3. Type CppCheckSD.exe and a valid command and press Enter, like 
       "D:\CppCheckSD.exe -d c:\", you can see all following information.

    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    >>                 SECURITY INFORMATION                >>
    >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

    object name ........ c:\
    object type ........ directory
    sd ................. O:S-1-5-80-956008885-3418522649-1831038044-1853292631-2271478464G:S-1-5-80-9560
    08885-3418522649-1831038044-1853292631-2271478464D:PAI(A;;FA;;;BA)(A;OICIIO;GA;;;BA)(A;;FA;;;SY)(A;O
    ICIIO;GA;;;SY)(A;OICI;0x1200a9;;;BU)(A;OICIIO;SDGXGWGR;;;AU)(A;;LC;;;AU)S:(ML;OINPIO;NW;;;HI)
    length ............. 282


/////////////////////////////////////////////////////////////////////////////
Implementation:

1.In order to obtain a security descriptor to any kernel object, you need
to obtain a handle with the following access permissions:

    READ_CONTROL

If you specify the auditing switch, you also need the following access 
permissions:

    ACCESS_SYSTEM_SECURITY

and you need to enable the SE_SECURITY_NAME privilege before requesting this
access right.  


2. Some Kernel Objects, the security APIs will internally obtain the handle for the
targeted object while others you have to obtain the handle yourself.  There are 2
functions for doing this:

    DumpObjectWithHandle()
    DumpObjectWithName()

By default we are requesting the following security information:

    a. Owner
    b. Group
    c. Dacl
    d. Label

If you request to see the audit aces, you also need to specify:

    a. Sacl

3. Once you have obtained the Security Descriptor, by default we are displaying 
the Security Descriptor in SDDL format using the following function:

    ConvertSecurityDescriptorToStringSecurityDescriptor()

This is done in the DumpSD function.  To obtain the full binary security 
descriptor, you need to use various helper functions in the sample.  See 
SD.cpp for more information.


/////////////////////////////////////////////////////////////////////////////
References:

Security Descriptors:
http://msdn.microsoft.com/en-us/library/aa379563.aspx

SDDL:
http://msdn.microsoft.com/en-us/library/aa379567.aspx


/////////////////////////////////////////////////////////////////////////////