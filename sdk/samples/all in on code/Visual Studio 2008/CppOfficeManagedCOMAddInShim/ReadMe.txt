=================================================================================
    Class Library APPLICATION : CppOfficeManagedCOMAddInShim Project Overview
=================================================================================

/////////////////////////////////////////////////////////////////////////////
Overview:

When we build a managed Office extension of any kind, we should ensure that
our extension is isolated from other extensions that might be loaded into 
the application. The standard way to isolate our extension is to build a 
custom COM shim by using the COM Shim Wizard, a set of Visual Studio project
wizards that helps you construct shims quickly and easily.


/////////////////////////////////////////////////////////////////////////////
Project Relation:

CppOfficeManagedCOMAddInShim -> CSOfficeSharedAddIn
CppOfficeManagedCOMAddInShim shims the managed COM AddIn CSOfficeSharedAddIn

ManagedAggregator -> CppOfficeManagedCOMAddInShim
ManagedAggregator loads and instantiates the add-in object, so that it can 
correctly aggregate this with the shim.


/////////////////////////////////////////////////////////////////////////////
Creation:

Step1. Download the COM Shim Wizard template from the Microsoft download
site, and install it so we can find the Shim AddIn template in Visual Studio,
http://www.microsoft.com/downloads/details.aspx?FamilyId=3E43BF08-5008-4BB6-AA85-93C1D902470E&displaylang=en

Step2. Create an Office COM Addin(Shared AddIn) to be shimed, and compile it. 
In this example, we use CSOfficeSharedAddIn as the target managed COM AddIn.
It's a good practice to sign the managed COM AddIn dll with strong name.

Step3. Create a new project. In the New Project dialog, navigate to the 
Visual C++ / COMShims node. Select the AddIn Shim project in right panel, 
name it as CppOfficeManagedCOMAddInShim and click OK.

Step4. The COM Shim Wizard pops up. In the first page of the wizard, specify 
the output assembly of the managed COM AddIn project, and press Next button. 
(We may receive a warning dialog saying that the assembly needs to be signed 
as strong name if the target assembly was not signed. Just click Yes to 
ignore it)

Step5. The second page is for configuration when our AddIn implements a 
secondary extensibility interface. We do not need this in this sample. Just 
press Next

Step6. The third page is about Shared Add-in Details. In the page we can 
specify:

	Add-in Description
	Add-in Friendly Name
	Whether to load the Add-in when the host Office application starts
	Whether to install the Add-in to all users
	Add-in's host Office application
	
In this example, we check the Microsoft Word, Visio, Publisher, Project, 
PowerPoint, Outlook, FrontPage, Excel, and Access checkboxes and, press Next 
to go on.

Step7. The last is a summary page. Click Finish to create the AddIn Shim 
project. Accompanied with CppOfficeManagedCOMAddInShim, the wizard creates a 
.NET class library project named ManagedAggregator. The generated project 
also includes a final build task that runs Regsvr32.exe on the target DLL to 
register the shim, so you do not need to register it manually. The final 
build task copies the ManagedAggregator.dll into the target folder for the 
shim. It also copies the managed add-in assembly that you specified into the 
target folder for the shim, along with the configuration file for the add-in. 
The project dependencies are set so that the ManagedAggregator is built first, 
before the shim. If you add a shim project to an existing add-in solution, 
you should adjust the build dependencies, as appropriate. 

Step8. After the creation, compile the solution. You will find 
CSOfficeSharedAddIn in Office applications' COM Addins Dialog. And the 
location points to CppOfficeManagedCOMAddInShim.dll instead of mscoree.dll.


/////////////////////////////////////////////////////////////////////////////
Deployment:

Step1. Navigate to the output folder of the CppOfficeManagedCOMAddInShim 
project. We can see the generated files:

	CppOfficeManagedCOMAddInShim.dll
	ManagedAggregator.dll
	CSOfficeSharedAddIn.dll

Step2. Copy the above three files to the target computer where you want to 
deploy the Add-in.

Step3. On the target computer, start a command prompt as administrator.

Step4. Enter the command "regsvr32 CppOfficeManagedCOMAddInShim.dll" to 
register the shim dll.

Step5. Open an Office application to verify the AddIn is shimed by 
CppOfficeManagedCOMAddInShim.dll.


If you build a setup project for your add-in, you should add the primary 
output of both the shim and the ManagedAggregator projects to your setup. You 
should also change the value of the Register property for all three project 
outputs. Set this to vsdrpCOM for the shim DLL, and vsdrpDoNotRegister for 
both the original add-in DLL and the ManagedAggregator DLL. 


/////////////////////////////////////////////////////////////////////////////
References:

MSDN: Isolating Microsoft Office Extensions with the COM Shim Wizard Version 
2.3.1
http://msdn.microsoft.com/en-us/library/bb508939.aspx

COM Shim Wizard Author's blog:
http://blogs.msdn.com/andreww/archive/2007/07/05/updated-com-shim-wizards.aspx

COM Shim Wizard download page:
http://www.microsoft.com/downloads/details.aspx?FamilyId=3E43BF08-5008-4BB6-AA85-93C1D902470E&displaylang=en


/////////////////////////////////////////////////////////////////////////////
