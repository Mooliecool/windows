================================================================================
				Windows APPLICATION: VBCustomCodeAnalysisRule                        
===============================================================================
/////////////////////////////////////////////////////////////////////////////
Summary:
The sample demonstrates how to create, deploy and run custom Code Analysis rules.

You can use Code Analysis in Visual Studio 2010 Premium and Visual Studio 2010 Ultimate
to discover potential issues in your code. The rules in this sample are used to check
the names of fields, properties and methods.

/////////////////////////////////////////////////////////////////////////////
Setup and Removal:

--------------------------------------
In the Development Environment

A. Setup

Navigate to the output folder
1. Copy VBCustomCodeAnalysisRule.dll to
C:\Program Files (x86)\Microsoft Visual Studio 10.0\Team Tools\Static Analysis Tools\
FxCop\Rules [For 64bit OS] or C:\Program Files\Microsoft Visual Studio 10.0\Team Tools\
Static Analysis Tools\FxCop\Rules [For 32 bit OS]

2. Copy VBCustomCodeAnalysisRule.ruleset to 
C:\Program Files (x86)\Microsoft Visual Studio 10.0\Team Tools\Static Analysis Tools\
Rule Sets [For 64bit OS] or C:\Program Files (x86)\Microsoft Visual Studio 10.0\Team Tools\
Static Analysis Tools\Rule Sets [For 32 bit OS]

B. Removal

Delete above 2 files.


--------------------------------------
In the Deployment Environment

A. Setup

Install VBCustomCodeAnalysisRuleSetup.msi, the output of the VBCustomCodeAnalysisRuleSetup 
setup project.


B. Removal

Uninstall VBCustomCodeAnalysisRuleSetup.msi, the output of the VBCustomCodeAnalysisRuleSetup
setup project. 



////////////////////////////////////////////////////////////////////////////////
Demo:

Step1. Open this project in  Visual Studio 2010 Premium or Visual Studio 2010 Ultimate. 
        
Step2. Build the solution. 

Step3. Right click the project VBCustomCodeAnalysisRuleSetup in Solution Explorer, 
       and choose "Install".

       NOTE: You can also copy the files follow the steps in the "Setup and Removal" section.

Step4. Open another VS2010 instance, create a VB class library project TestCA.vbproj, 
       add following class.

	      Public Class ClassA

              Private WrongFieldName As Integer
              Private rightFieldName As Integer
          
              Public Property wrongPropertyName() As Integer
              Public Property RightPropertyName() As Integer
          
              Public Sub wrongMethodName()
              End Sub
              Public Sub RightMethodName()
              End Sub
          
          End Class

Step5. Open the properties page of the project TestCA, in the Code Analysis tab, select
       VBCustomCodeAnalysisRule.

Step6. In the VS, click Analyze=> Run Code Analysis on TestCA. You will get following 
       warnings in the Error List Window.

	   CCAR0001 : Naming : The name of the field WrongFieldName in class TestCA.ClassA 
	   should start with a lowercase character.	

       CCAR0002 : Naming : The name of the method wrongMethodName in class TestCA.ClassA
	   should start with a uppercase character.	

       CCAR0003 : Naming : The name of the property wrongPropertyName in class TestCA.ClassA 
	   should start with a uppercase character.	
       


/////////////////////////////////////////////////////////////////////////////
Code Logic:

A. Create the project and add references

In Visual Studio 2010, create a Visual C# / Windows / Class Library project 
named "VBCustomCodeAnalysisRule". 

Add references FxCopSdk.dll and Microsoft.Cci.dll. These two assemblies are located in
the FxCop folder. FxCop 10.0 is included in Windows SDK 7.1. You can download it in the 
following link.
http://www.microsoft.com/downloads/en/details.aspx?FamilyID=35aeda01-421d-4ba5-b44b-543dc8c33a20&displaylang=en

B. Implement Code Analysis Rules FieldNamingRule, MethodNamingRule, and PropertyNamingRule.

A custom Code Analysis rule is a sealed class which inherits the class 
Microsoft.FxCop.Sdk.BaseIntrospectionRule. Override the method 
public ProblemCollection Check(Member member) to check the members.

    sealed class PropertyNamingRule : BaseIntrospectionRule
    {
        public PropertyNamingRule()
            : base( "PropertyNamingRule", "VBCustomCodeAnalysisRule.Rules",
                typeof(FieldNamingRule).Assembly)
        {}
      
        public override ProblemCollection Check(Member member)
        {         
            if (member is PropertyNode)
            {
                PropertyNode property = member as PropertyNode;

                if (property.Name.Name[0] < 'A' || property.Name.Name[0] > 'Z')
                {
                    this.Problems.Add(new Problem(
                       this.GetNamedResolution(
                       "UppercaseProperty", 
                       property.Name.Name,
                       property.DeclaringType.FullName)));
                }
            }

            return this.Problems;
        }
    }


C. Define the rule in Rules.xml. 
   
   This file defines metadata for all of the rules in the assembly. In Visual Studio, add
   a file named Rules.xml and then mark the file as an "Embedded Resource" in the properties
   window. The xml is like 

<Rules>
  <Rule TypeName="FieldNamingRule" Category="Naming" CheckId="CCAR0001">
    <Name>Field name should start with a lowercase character</Name>
    <Description> The name of a field in a class should start with a lowercase character.  </Description>
    <Resolution Name="LowercaseField"> The name of the field {0} in class {1} should start with a lowercase character. </Resolution>
    <MessageLevel Certainty="99">Warning</MessageLevel>
    <Message Certainty="99">Warning</Message>
    <FixCategories>NonBreaking</FixCategories>
    <Owner></Owner>
    <Url></Url>
    <Email></Email>
  </Rule>
  ...

   
D. Define a new Rule Set.
A new feature in Visual Studio 2010 is called rule sets. Rule sets are a new way of 
configuring which rules should be run during analysis. A rule set is like 

<?xml version="1.0" encoding="utf-8"?>
<RuleSet Name="VBCustomCodeAnalysisRule" Description=" " ToolsVersion="10.0">
  <Rules AnalyzerId="Microsoft.Analyzers.ManagedCodeAnalysis" RuleNamespace="Microsoft.Rules.Managed">
    <Rule Id="CCAR0001" Action="Warning" />
    <Rule Id="CCAR0002" Action="Warning" />
    <Rule Id="CCAR0003" Action="Warning" />
  </Rules>
</RuleSet>

E. Deploying the Rule with a setup project.

  To add a deployment project, on the File menu, point to Add, and then click New Project. 
  In the Add New Project dialog box, expand the Other Project Types node, expand the Setup
  and Deployment Projects, click Visual Studio Installer, and then click Setup Project. In
  the Name box, type VBCustomCodeAnalysisRuleSetup. Click OK to create the project. 
  
  1 Right-click the setup project, and choose View / File System. 
  
  2 In the File System window, right click the File System on Target Machine, and choose Add
    Special Folder=> Program Files Folder
  
  3 Create folder "Microsoft Visual Studio 10.0\Team Tools\Static Analysis Tools\Rule Sets" 
    under Program Files Folder. Add the Primry output from CSCustomCodeAnalysis to the folder.
	
  4 Create folder "Microsoft Visual Studio 10.0\Team Tools\Static Analysis Tools\FxCop\Rules"
    under Program Files Folder. Add VBCustomCodeAnalysisRule.ruleset to the folder.
  
  Build the setup project. If the build succeeds, you will get a .msi file and a Setup.exe file.
  You can distribute them to your users to install or uninstall these rules. 
/////////////////////////////////////////////////////////////////////////////
References:
http://msdn.microsoft.com/en-us/library/microsoft.build.evaluation.project.aspx
http://www.binarycoder.net/fxcop/html/doccomments.html
/////////////////////////////////////////////////////////////////////////////