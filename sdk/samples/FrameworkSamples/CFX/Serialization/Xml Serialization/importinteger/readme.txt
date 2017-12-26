SchemaImporterExtension Import Integer Sample
=============================================
     Shows how to write a SchemaImporterExtension to import the XML Schema integer types as long and ulong instead of as string.


Sample Language Implementations
===============================
     This sample is available in the following language implementations:
     C#


To build the sample using the command prompt:
=============================================
     1. Open the Command Prompt window and navigate to the Technologies\Serialization\Xml Serialization\SchemaImporterExtension\ImportInteger directory.
     2. Type msbuild [Solution Filename].


To build the sample using Visual Studio:
=======================================
     1. Open Windows Explorer and navigate to the Technologies\Serialization\Xml Serialization\SchemaImporterExtension\ImportInteger directory.
     2. Double-click the icon for the .sln (solution) file to open the file in Visual Studio.
     3. In the Build menu, select Build Solution.
     The application will be built in the default \bin or \bin\Debug directory.

     Additional things to consider when building this sample are:
     1.  Generate a strong name for the assembly
     2.  Add the assembly to the GAC
     3.  Add the assembly to machine.config in a schemas.xml.serialization/schemaImporterExtension section


To run the sample:
=================
     1. Navigate to the directory that contains the new executable, using the command prompt or Windows Explorer.
     2. Type [ExecutableFile] at the command line, or double-click the icon for [SampleExecutable] to launch it from Windows Explorer.

     The sample can be built with limited permission, but installing requires admin privileges since the assembly must be added to the GAC and machine.config must be edited.

     Example entry in machine.config:

     system.xml.serialization
     	schemaImporterExtensions
     		add name="RandomString" type="Microsoft.Samples.Xml.Serialization.SchemaImporterExtension.ImportInteger, ImportInteger, Version=0.0.0.0, Culture=neutral, PublicKeyToken=3c3789dee90b3265"
     	schemaImporterExtensions
     system.xml.serialization


Remarks
=================
     1.  Run xsd.exe, wsdl.exe, or Add Web Reference on WSDL that uses xs:integer, xs:negativeInteger, xs:nonNegativeInteger, xs:positiveInteger, or xs:nonPositiveInteger
2.  Note that the generated class uses long or ulong instead of string for the XML Schema integer types

