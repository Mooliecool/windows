=============================================================================
           CONSOLE APPLICATION : CppHostCLR Project Overview
=============================================================================

/////////////////////////////////////////////////////////////////////////////
Summary:

The Common Language Runtime (CLR) allows a level of integration between 
itself and a host. This C++ code sample demonstrates using the Hosting 
Interfaces of .NET Framework 4.0 to host a specific version of 
CLR in the process, load a .NET assembly, and invoke the types in the 
assembly.

The code sample also demonstrates the new In-Process Side-by-Side feature in 
.NET Framework 4. The .NET Framework 4 runtime, and all future runtimes, are 
able to run in-process with one another. .NET Framework 4 runtime and beyond 
are also able to run in-process with any single older runtime. In ther words, 
you will be able to load 4, 5 and 2.0 in the same process, but you will not 
be able to load 1.1 and 2.0 in the same process. The code sample hosts .NET 
runtime 4.0 and 2.0 side by side, and loads a .NET 2.0 assembly into the two 
runtimes.


/////////////////////////////////////////////////////////////////////////////
Sample Relation:

CppHostCLR -> CSClassLibrary
CppHostCLR hosts the .NET runtime 4.0, loads the .NET 4.0 assembly 
CSClassLibrary and invokes its type.

CppHostCLR -> CSNET2ClassLibrary
CppHostCLR hosts the .NET runtime 4.0 and 2.0, loads the .NET 2.0 assembly 
CSNET2ClassLibrary into the runtimes, and invokes the types in the assembly.


/////////////////////////////////////////////////////////////////////////////
Demo:

The following steps walk through a demonstration of the CLR Hosting sample.

Step1. After you successfully build the sample project, and the dependent 
.NET class library projects (CSClassLibrary and CSNET2ClassLibrary) in Visual 
Studio 2010, you will get an application and two libraries: 
CppHostCLR.exe, CSClassLibrary.dll, and CSNET2ClassLibrary.dll. Make sure 
that the files are in the same folder.

Step2. Run the application in a command prompt. The application demonstrates 
the CLR In-Process Side-by-Side feature that is new in .NET 4 first. It hosts 
the .NET runtime 4.0 first and loads a .NET 2.0 assembly into the runtime to 
invoke its types. Then the application hosts the .NET runtime 2.0 side by 
side with the .NET runtime 4.0 and loads the same .NET 2.0 assembly into the 
.NET 4.0 runtime. If the operations succeed, the application prints the 
following content in the console.

    Load and start the .NET runtime v4.0.30319
    Load the assembly CSNET2ClassLibrary
    Call CSNET2ClassLibrary.CSSimpleObject.GetStringLength("HelloWorld") => 10
    Call CSNET2ClassLibrary.CSSimpleObject.ToString() => 0.00

    Load and start the .NET runtime v2.0.50727
    Load the assembly CSNET2ClassLibrary
    Call CSNET2ClassLibrary.CSSimpleObject.GetStringLength("HelloWorld") => 10
    Call CSNET2ClassLibrary.CSSimpleObject.ToString() => 0.00

    Presss ENTER to continue ...

You can verify that both .NET 2.0 and .NET 4.0 runtimes are loaded by using 
Process Explorer (http://technet.microsoft.com/en-us/sysinternals/bb896653.aspx).
In Lower Pane View / DLLs of the tool, you can see all modules loaded in the 
process. If clr.dll (the .NET 4 runtime module) and mscorwks.dll (the .NET 2 
runtime module) are in the list, both .NET 2.0 and .NET 4.0 runtimes are 
loaded.

Step3. Press ENTER to continue. The application will host the .NET runtime 
4.0 and use the ICLRRuntimeHost interface that was provided in .NET v2.0 to 
load a .NET 4.0 assembly and invoke its type.

    Load and start the .NET runtime v4.0.30319
    Load the assembly CSClassLibrary.dll
    Call CSClassLibrary.CSSimpleObject.GetStringLength("HelloWorld") => 10


/////////////////////////////////////////////////////////////////////////////
Implementation:




/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Hosting Overview
http://msdn.microsoft.com/en-us/library/dd380850.aspx

CLR Inside Out: In-Process Side-by-Side
http://msdn.microsoft.com/en-us/magazine/ee819091.aspx


/////////////////////////////////////////////////////////////////////////////
