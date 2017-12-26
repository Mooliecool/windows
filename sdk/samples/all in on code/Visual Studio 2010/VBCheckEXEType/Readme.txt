================================================================================
	   Windows APPLICATION: VBCheckEXEType Overview                        
===============================================================================
/////////////////////////////////////////////////////////////////////////////
Summary:
The sample demonstrates how to check an executable file type. Given an exe or a dll,
the sample application detects:

1. exe type (console or GUI, or other exe type) by checking the subsystem flag in PE
2. Is it a .NET assembly? 
    If no, 
        - detect the exe bitness (x86 or x64)
    If yes, 
        - detect the exe bitness (ANY CPU, x86, x64)
        - detect compiled .NET runtime version 
        - print the full name of the assembly 
		(e.g. System, Version=2.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089, processorArchitecture=MSIL)
   
////////////////////////////////////////////////////////////////////////////////
Demo:
Step1. Build the sample project in Visual Studio 2010.

Step2. Run VBCheckEXEType.exe. This application will show following help text.
Please type the exe file path:
<Empty to exit>

Step3. Type a valid path of a native executable and press Enter, like 
       "D:\NativeConsole32.exe", you can see all following information.

ConsoleApplication: True
.NetApplication: False
32bit application: True

Step4. Type a valid path of a .Net executable and press Enter, like 
       "D:\NetWinFormAnyCPU.exe", you can see all following information.

ConsoleApplication: False
.NetApplication: True
Compiled .NET Runtime: v4.0.30319
Full Name: NetWinFormAnyCPU, Version=1.0.0.0, Culture=neutral, PublicKeyToken=neutral, processorArchitecture=MSIL


/////////////////////////////////////////////////////////////////////////////
Code Logic:

First, create an ExecutableFile class that represents an executable file. It could get 
the image file header, image optional header and data directories from the image file. 
Form these headers, we can get whether this is a console application, whether this is 
a .Net application and whether this is a 32bit native application. 

Note: The IMAGE_OPTIONAL_HEADER structures of 32bit and 64 bit application are 
      different. The difference is that 64 bit application does not have BaseOfData
      field and the data type of ImageBase/SizeOfStackReserve/SizeOfStackCommit/
      SizeOfHeapReserve/SizeOfHeapCommit is UInt64. 

Second, to generate the full display name of .NET application, we can use the fusion API. 

  Char[] buffer = new Char[1024];

  // Get the IReferenceIdentity interface.
  Fusion.IReferenceIdentity referenceIdentity =
     Fusion.NativeMethods.GetAssemblyIdentityFromFile(ExeFilePath,
     ref Fusion.NativeMethods.ReferenceIdentityGuid) as Fusion.IReferenceIdentity;
  Fusion.IIdentityAuthority IdentityAuthority = Fusion.NativeMethods.GetIdentityAuthority();  
  
  IdentityAuthority.ReferenceToTextBuffer(0, referenceIdentity, 1024, buffer);

  string fullName = new string(buffer);

Third, to detect the compiled .NET runtime version, we can use the hosting API.

  object metahostInterface=null;
  Hosting.NativeMethods.CLRCreateInstance(
      ref Hosting.NativeMethods.CLSID_CLRMetaHost,
      ref Hosting.NativeMethods.IID_ICLRMetaHost, 
      out metahostInterface);

  if (metahostInterface == null || !(metahostInterface is Hosting.IClrMetaHost))
  {
      throw new ApplicationException("Can not get IClrMetaHost interface.");
  }

  Hosting.IClrMetaHost ClrMetaHost = metahostInterface as Hosting.IClrMetaHost;
  StringBuilder buffer=new StringBuilder(1024);
  uint bufferLength=1024;          
  ClrMetaHost.GetVersionFromFile(this.ExeFilePath, buffer, ref bufferLength);
  string runtimeVersion = buffer.ToString(); 
/////////////////////////////////////////////////////////////////////////////
References:

An In-Depth Look into the Win32 Portable Executable File Format
http://msdn.microsoft.com/en-us/magazine/cc301805.aspx

Exploring pe file headers using managed code
http://blogs.msdn.com/b/kstanton/archive/2004/03/31/105060.aspx

Getting the full display name of an assembly given the path to the manifest file
http://blogs.msdn.com/b/junfeng/archive/2005/09/13/465373.aspx

IReferenceIdentity Interface
http://msdn.microsoft.com/en-us/library/ms231949.aspx

IIdentityAuthority Interface
http://msdn.microsoft.com/en-us/library/ms231265(VS.80).aspx

GetIdentityAuthority Function
http://msdn.microsoft.com/en-us/library/ms231607(VS.80).aspx

GetAssemblyIdentityFromFile Function
http://msdn.microsoft.com/en-us/library/ms230508.aspx

CLRCreateInstance Function
http://msdn.microsoft.com/en-us/library/dd537633.aspx

ICLRMetaHost::GetVersionFromFile Method
http://msdn.microsoft.com/en-us/library/dd233127.aspx